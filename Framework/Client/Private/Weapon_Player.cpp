#include "pch.h"
#include "Weapon_Player.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "StateMachine.h"

#include "Player.h"
#include "Hand_Player.h"
#include "Shell_Part.h"
#include "Shell_World.h"

#include "IDamaged.h"

#include "UI_Manager.h"

CWeapon_Player::CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CWeapon_Player::CWeapon_Player(const CWeapon_Player& Prototype)
	: super(Prototype)
{
}

HRESULT CWeapon_Player::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Player::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_pOwner = pDesc->pOwner;

	m_tWeaponStat.iLevel = pDesc->iLevel;
	m_tWeaponStat.fDamage = pDesc->fDamage;

	if (pDesc == nullptr)
	{
		MSG_BOX("Failed To Create : CWeapon_Player");
		return E_FAIL;
	}

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)) ||
		FAILED(Ready_Components()))
		return E_FAIL;

	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_BEACH)
		m_bActive = false;

	m_pColliderCom->Set_Active(false);

	return S_OK;
}

void CWeapon_Player::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	super::Priority_Update(fTimeDelta);
}

void CWeapon_Player::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	super::Update(fTimeDelta);
}

void CWeapon_Player::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif

	super::Late_Update(fTimeDelta);
}

void CWeapon_Player::Final_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	super::Final_Update(fTimeDelta);
}

HRESULT CWeapon_Player::Render()
{
	if (!m_bActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iShaderPass = 0;
		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

			if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
			return E_FAIL;

		m_pShaderCom->Begin(iShaderPass);
		m_pModelCom->Render(i);
	}

	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

const FShellStat* CWeapon_Player::Get_ShellStat_Const() const
{
	if (m_pShell == nullptr || !m_bActive)
		return nullptr;

	return m_pShell->Get_ShellStat_Const();
}

HRESULT CWeapon_Player::Attach_Shell(SHELL eShell, _float fHP)
{
	Attach_Shell_NoSound(eShell, fHP);
	m_pOwner->Play_PlayerSound(L"Hammer_Get");

	return S_OK;
}

HRESULT CWeapon_Player::Attach_Shell_NoSound(SHELL eShell, _float fHP)
{
	if (!m_bActive)
		return S_OK;

	if (m_tWeaponStat.bHasShell)
		Deattach_Shell();

	CShell_Part::DESC tShellDesc{};
	tShellDesc.ShellItemDesc.ItemDesc.GameObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
	tShellDesc.ShellItemDesc.eShell = eShell;
	tShellDesc.ShellItemDesc.fShellCurrHP = fHP;
	tShellDesc.pOwner = this;

	if (FAILED(Add_PartObject
	(
		L"Part_Shell",
		LEVEL_STATIC,
		L"Prototype_GameObject_Shell_Part",
		&tShellDesc
	)))
		return E_FAIL;

	m_pShell = static_cast<CShell_Part*>(Find_PartObject({ L"Part_Shell" }));

	m_pOwner->Get_PlayerStat()->bHasHammer = true;
	m_tWeaponStat.bHasShell = true;

	CTransform* pTransform = static_cast<CTransform*>(m_pShell->Find_Component(g_strTransformTag));
	pTransform->Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));
	pTransform->Locate(0.45f, 0.7f, 0.f);

	return S_OK;
}

void CWeapon_Player::Deattach_Shell()
{
	if (!m_bActive)
		return;

	Delete_PartObject({ L"Part_Shell" });

	m_pOwner->Get_PlayerStat()->bHasHammer = false;

	m_tWeaponStat.bHasShell = false;
	m_pShell = nullptr;
}

void CWeapon_Player::Collider_Toggle(_bool bToggle)
{
	m_pColliderCom->Set_Active(bToggle);
}

HRESULT CWeapon_Player::Upgrade_Weapon()
{
	if (m_tWeaponStat.iLevel >= 6) 
		return S_OK;

	m_tWeaponStat.iLevel++;
	m_tWeaponStat.fDamage = 10.f * (_float)m_tWeaponStat.iLevel;

	Delete_Component(L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom));
	Safe_Release(m_pModelCom);

	Create_Model();

	m_pOwner->Get_PlayerHand()->Set_Damage(m_tWeaponStat.fDamage);

	return S_OK;
}

void CWeapon_Player::MoveToSocket(SOCKET eSocket)
{
	m_pSocketMatrix = m_pSocketMatrices[static_cast<_uint>(eSocket)];

	_float3 vScale = m_vTransforms[static_cast<_uint>(eSocket)][0];
	_float3 vRotation = m_vTransforms[static_cast<_uint>(eSocket)][1];
	_float3 vPosition = m_vTransforms[static_cast<_uint>(eSocket)][2];

	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Rotate(vRotation.x, vRotation.y, vRotation.z);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vPosition));
}

void CWeapon_Player::Add_Socket(SOCKET eSocket, const _char* szFilePath)
{
	CModel* pModel = static_cast<CModel*>(m_pOwner->Find_Component(L"Com_Model"));

	Load_From_File(eSocket, szFilePath, pModel);
}

HRESULT CWeapon_Player::Create_Model()
{
	_wstring strModelTag{};

	switch (m_tWeaponStat.iLevel)
	{
	case 1:
		strModelTag = L"Prototype_Component_Model_Fork";
		break;
	case 2:
		strModelTag = L"Prototype_Component_Model_MiddleFork";
		break;
	case 3:
		strModelTag = L"Prototype_Component_Model_GoldFork";
		break;
	case 4:
		strModelTag = L"Prototype_Component_Model_GoldFork";
		break;
	case 5:
		strModelTag = L"Prototype_Component_Model_GoldFork";
		break;
	default:
		break;
	}

	// Model
	if (FAILED(Add_Component
	(
		LEVEL_STATIC,
		strModelTag,
		L"Com_Model",
		reinterpret_cast<CComponent**>(&m_pModelCom)
	)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Player::Load_From_File(SOCKET eSocket, const _char* szFilePath, CModel* pModel)
{
	_string strLoadFile = _string(szFilePath);
	_wstring strFilePath = CUtils::ConvertToWideChar(strLoadFile);

	_bool bResult{};

	_ulong dwByte{};
	HANDLE hFile{};
	hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed To Load PartObject Data");
		return E_FAIL;
	}

	_char szSocketBone[MAX_PATH]{};

	bResult = ReadFile(hFile, &szSocketBone, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

	_float3 vScale{}, vRotation{}, vPosition{};

	bResult = ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);

	m_vTransforms[static_cast<_uint>(eSocket)][0] = vScale;
	m_vTransforms[static_cast<_uint>(eSocket)][1] = vRotation;
	m_vTransforms[static_cast<_uint>(eSocket)][2] = vPosition;

	CloseHandle(hFile);

	if (!strcmp(szSocketBone, "NoSocket"))
	{
		_float4x4 matSocket{};
		XMStoreFloat4x4(&matSocket, XMMatrixIdentity());

		m_pSocketMatrices[static_cast<_uint>(eSocket)] = &matSocket;
	}
	else
		m_pSocketMatrices[static_cast<_uint>(eSocket)] = &pModel->Get_BoneCombinedMatrix_By_Name(szSocketBone);

	return S_OK;
}

void CWeapon_Player::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case static_cast<_uint>(COLLISION_GROUP::MONSTER):
	{
		IDamaged* pDamaged = dynamic_cast<IDamaged*>(pOther);
		if (pDamaged != nullptr)
		{
			m_pGameInstance->Call_Lag(0.01f, 0.08f);
			_matrix CombinedWorldMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
			_vector vPos{}, vUp{}, vLook{}, vCamPos{}, vCamLook;
			vPos = CombinedWorldMatrix.r[(_uint)CTransform::STATE::POSITION];
			vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			vUp = CombinedWorldMatrix.r[(_uint)CTransform::STATE::UP];
			CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PTCL_HITSPARK, vPos, vLook, false, nullptr);

			vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
			vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
			vLook = XMLoadFloat4x4(m_pParentWorldMatrix).r[(_uint)CTransform::STATE::LOOK];
			CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vLook, vCamLook, false, nullptr);

			_uint iAttackPower = m_pOwner->Get_PlayerStat()->iAttackPower;
			_float fDamageFactor = m_pOwner->Get_PlayerStat()->fDamageFactor;
			_float fFinalDamage = m_tWeaponStat.fDamage * ((5.f * (_float)(iAttackPower)) / 100.f) * (fDamageFactor);

			IDamaged::DAMAGE eDamageType = m_pOwner->Get_PlayerStat()->eDamageType;

			pDamaged->Damaged(fFinalDamage, eDamageType, m_pOwner);
			m_pOwner->Heal_Umami(fFinalDamage);

			if (m_tWeaponStat.bHasShell)
			{
				m_pShell->Damaged(fFinalDamage * 0.1f, IDamaged::DAMAGE::DAMAGE_NORMAL, pOther);
				//m_pShell->Damaged(fFinalDamage * 100.f, IDamaged::DAMAGE::DAMAGE_NORMAL, pOther);
				if (m_pShell->Get_ShellStat_Const()->fShellCurrHP == 0.f)
					Deattach_Shell();
			}
		}
	}
	break;
	case(_uint)COLLISION_GROUP::SHELL:
	{
		if (m_pOwner->Get_PlayerStat()->bCanBeHammer && !m_pOwner->Get_PlayerStat()->bHasHammer)
		{
			CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
			pUIManager->Hide_Interaction();

			CShell_World* pShellWorld = static_cast<CShell_World*>(pOther);
			SHELL eShell = pShellWorld->Get_ShellStat_Const()->eShell;
			_float fHP = pShellWorld->Get_ShellStat_Const()->fShellCurrHP;

			Attach_Shell(eShell, fHP);
			m_pOwner->Get_StateMachine()->ChangeState(L"HammerPick");

			m_pGameInstance->Delete_GameObject
			(
				m_pGameInstance->Get_CurrentLevelIndex(),
				L"Layer_Shell",
				pShellWorld
			);
		}
	}
	break;
	default:
		break;
	}
}

void CWeapon_Player::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CWeapon_Player::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CWeapon_Player::Ready_Components()
{
	Create_Model();

	// Com_Shader
	if (FAILED(super::Add_Component
	(
		LEVEL_STATIC,
		TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom)
	)))
		return E_FAIL;

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = static_cast<_uint>(COLLISION_GROUP::PLAYER_WEAPON);

		Desc.fRadius = 1.f;
		Desc.vCenter = { 0.f, 1.f, 0.f };

		if (FAILED(Add_Component
		(
			LEVEL_STATIC,
			TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider",
			reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc
		)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Player::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}

CWeapon_Player* CWeapon_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Player* pInstance = new CWeapon_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CWeapon_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CWeapon_Player::Clone(void* pArg)
{
	CWeapon_Player* pInstance = new CWeapon_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CWeapon_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Player::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}