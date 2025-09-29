#include "pch.h"
#include "Shell_Part.h"

#include "GameInstance.h"

#include "Player.h"
#include "ShellSkill.h"
#include "Weapon_Player.h"

CShell_Part::CShell_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CShell_Part::CShell_Part(const CShell_Part& Prototype)
	: super(Prototype)
{
}

HRESULT CShell_Part::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShell_Part::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	m_pOwner = pDesc->pOwner;

	pDesc->ShellItemDesc.ItemDesc.GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);
	if (FAILED(super::Initialize(&pDesc->ShellItemDesc)))
		return E_FAIL;

	_string strFilePath = m_tShellData.strFilePath + ".data";

	if (FAILED(Load_From_File(strFilePath.c_str())))
		return E_FAIL;

	if (m_tShellStat.strSkillName == L"ShellSkill_RollOut")
	{
		_string strRollFilePath = m_tShellData.strFilePath + "_Roll.data";
		
		if (FAILED(Load_From_File(strRollFilePath.c_str())))
			return E_FAIL;
	}

	return S_OK;
}

void CShell_Part::Priority_Update(_float fTimeDelta)
{
	if(m_pSkillCom != nullptr)
		m_pSkillCom->Priority_Update(fTimeDelta);

	super::Priority_Update(fTimeDelta);
}

void CShell_Part::Update(_float fTimeDelta)
{
	if (m_pSkillCom != nullptr)
		m_pSkillCom->Update(fTimeDelta);

#ifdef _DEBUG
	if (m_pGameInstance->Key_Down(KEY::F8))
		Broken();
#endif

	super::Update(fTimeDelta);
}

void CShell_Part::Late_Update(_float fTimeDelta)
{
	if (m_pSkillCom != nullptr)
		m_pSkillCom->Late_Update(fTimeDelta);

	super::Late_Update(fTimeDelta);
}

void CShell_Part::Final_Update(_float fTimeDelta)
{
	_matrix SocketMatrix = XMMatrixIdentity();

	if (m_bSwap && m_pRollSocketMatrix != nullptr)
		SocketMatrix = XMLoadFloat4x4(m_pRollSocketMatrix);
	else if(!m_bSwap && m_pSocketMatrix != nullptr)
		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	if (m_pSkillCom != nullptr)
		m_pSkillCom->Final_Update(fTimeDelta);

	super::Final_Update(fTimeDelta);
}

HRESULT CShell_Part::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	CModel* pModel = m_pModelCom;
	if (m_bSwap && m_pRollModelCom != nullptr) 
		pModel = m_pRollModelCom;

	_uint iNumMeshes = static_cast<_uint>(pModel->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iShaderPass = 0;
		if (pModel->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

			if (FAILED(pModel->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

			if (FAILED(pModel->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
				return E_FAIL;
		}

		if (FAILED(pModel->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
			return E_FAIL;
		if (FAILED(pModel->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
			return E_FAIL;
		if (FAILED(pModel->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
			return E_FAIL;
		if (FAILED(pModel->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
			return E_FAIL;
		if (FAILED(pModel->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
			return E_FAIL;

		m_pShaderCom->Begin(iShaderPass);
		pModel->Render(i);
	}

	if (m_pSkillCom != nullptr)
		if (FAILED(m_pSkillCom->Render()))
			return E_FAIL;

	return S_OK;
}

_float CShell_Part::Get_SkillPoint()
{
	if (m_pSkillCom == nullptr)
		return 0.f;

	return m_pSkillCom->Get_SkillPoint();
}

void CShell_Part::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
{
	_vector vPos{}, vLook{};
	vPos = _mat(m_CombinedWorldMatrix).position._vector();
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DAMAGED_SHELLDEFEND, vPos, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);

	m_tShellStat.fShellCurrHP -= fDamage;
	if (m_tShellStat.fShellCurrHP <= 0.f)
	{
		m_tShellStat.fShellCurrHP = 0.f;
		Broken();
	}
}

void CShell_Part::Broken()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == (_uint)LEVEL_BEACH)
		return;

	// Effect
	_vector vPos{}, vLook{};
	vPos = _mat(m_CombinedWorldMatrix).position._vector();
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::SHELL_MESHINST_EXPLODE, vPos, vLook, false, nullptr);

	//CWeapon_Player* pWeapon = static_cast<CWeapon_Player*>(m_pOwner);
	//CGameObject* pTemp = pWeapon->Get_Owner();
	//CPlayer* pPlayer = static_cast<CPlayer*>(pTemp);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player", 0));

	_uint iType = (_uint)m_tShellStat.eShell;
	switch (iType)
	{
	case (_uint)SHELL::SHELL_SODACAN:
		pPlayer->Play_PlayerSound(L"Shell_Break_SodaCan");
		break;
	case (_uint)SHELL::SHELL_MIRRORBALL :
		pPlayer->Play_PlayerSound(L"Shell_Break_MirrorBall");
		break;
	case (_uint)SHELL::SHELL_DUCK:
		pPlayer->Play_PlayerSound(L"Shell_Break_Duck");
		break;
	case (_uint)SHELL::SHELL_TENNISBALL:
		pPlayer->Play_PlayerSound(L"Shell_Break_TennisBall");
		break;
	default:
		break;
	}

	pPlayer->Play_PlayerSound(L"Shell_Break_Base");
	pPlayer->Player_CameraShake_Rotation(0.03f, 20.f, 0.3f);
	m_pGameInstance->Call_Lag(0.05f, 0.2f);
}

void CShell_Part::Use_Skill()
{
	if(m_pSkillCom != nullptr)
		m_pSkillCom->Use_Skill();
}

void CShell_Part::Swap_Model(_bool bSwap)
{
	m_bSwap = bSwap;

	if (m_bSwap)
	{
		m_pTransformCom->Set_Scale(m_vRollScale);
		m_pTransformCom->Rotate(m_vRollRotation.x, m_vRollRotation.y, m_vRollRotation.z);
		m_pTransformCom->Locate(m_vRollPosition);
	}
	else
	{
		m_pTransformCom->Set_Scale(m_vOriginScale);
		m_pTransformCom->Rotate(m_vOriginRotation.x, m_vOriginRotation.y, m_vOriginRotation.z);
		m_pTransformCom->Locate(m_vOriginPosition);
	}
}

HRESULT CShell_Part::Load_From_File(const _char* szFilePath)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pOwner);
	if (pPlayer == nullptr)
	{
		m_pSocketMatrix = nullptr;
		return S_OK;
	}

	_string strLoadFile = _string(szFilePath);
	_wstring strFilePath = CUtils::ConvertToWideChar(strLoadFile);

	_bool bResult{};

	_ulong dwByte{};
	HANDLE hFile{};
	hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed To Load Shell Data");
		return E_FAIL;
	}

	_char szSocketBone[MAX_PATH]{};
	bResult = ReadFile(hFile, &szSocketBone, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

	CModel* pModel = static_cast<CModel*>(m_pOwner->Find_Component(L"Com_Model"));

	if (m_pSocketMatrix == nullptr)
	{
		m_pSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name(szSocketBone);

		bResult = ReadFile(hFile, &m_vOriginScale, sizeof(_float3), &dwByte, nullptr);
		bResult = ReadFile(hFile, &m_vOriginRotation, sizeof(_float3), &dwByte, nullptr);
		bResult = ReadFile(hFile, &m_vOriginPosition, sizeof(_float3), &dwByte, nullptr);

		m_pTransformCom->Set_Scale(m_vOriginScale);
		m_pTransformCom->Rotate(m_vOriginRotation.x, m_vOriginRotation.y, m_vOriginRotation.z);
		m_pTransformCom->Locate(m_vOriginPosition);
	}
	else
	{
		m_pRollSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name(szSocketBone);

		bResult = ReadFile(hFile, &m_vRollScale, sizeof(_float3), &dwByte, nullptr);
		bResult = ReadFile(hFile, &m_vRollRotation, sizeof(_float3), &dwByte, nullptr);
		bResult = ReadFile(hFile, &m_vRollPosition, sizeof(_float3), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CShell_Part::Ready_Components()
{
	if (FAILED(super::Ready_Components()))
		return E_FAIL;

	// Com_Skill
	CShellSkill::DESC tShellSkillDesc{};
	tShellSkillDesc.pOwner = this;
	tShellSkillDesc.fSkillPoint = m_tShellStat.fSkillPoint;

	if (m_tShellStat.strSkillName == L"None")
		return S_OK;

	if (FAILED(Add_Component
	(
		LEVEL_STATIC,
		m_tShellData.strSkill,
		L"Com_Skill",
		reinterpret_cast<CComponent**>(&m_pSkillCom),
		&tShellSkillDesc
	)))
		return E_FAIL;

	if (m_tShellStat.strSkillName == L"ShellSkill_RollOut")
	{
		_wstring strRollName = m_tShellData.strModel + L"_Roll";
		if (FAILED(Add_Component
		(
			LEVEL_STATIC,
			strRollName,
			L"Com_RollModel",
			reinterpret_cast<CComponent**>(&m_pRollModelCom)
		)))
			return E_FAIL;
	}

	return S_OK;
}

CShell_Part* CShell_Part::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShell_Part* pInstance = new CShell_Part(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CShell_Part");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CShell_Part::Clone(void* pArg)
{
	CShell_Part* pInstance = new CShell_Part(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CShell_Part");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShell_Part::Free()
{
	super::Free();

	Safe_Release(m_pRollModelCom);
	Safe_Release(m_pSkillCom);
}