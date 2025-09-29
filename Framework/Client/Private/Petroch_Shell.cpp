#include "pch.h"
#include "Petroch_Shell.h"
#include "GameInstance.h"
#include "Collider.h"

#include "Boss_Petroch.h"
#include "Player.h"

CPetroch_Shell::CPetroch_Shell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPetroch_Shell::CPetroch_Shell(const CPetroch_Shell& Prototype)
	: super(Prototype)
{
}

HRESULT CPetroch_Shell::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPetroch_Shell::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (pDesc == nullptr)
	{
		MSG_BOX("Failed To Create : CPetroch_Shell");
		return E_FAIL;
	}
	else {
		m_fDamage = pDesc->fDamage;
		m_pOwner = pDesc->pOwner;
	}

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pDesc->pSocketMatrix != nullptr)
		m_pSocketMatrix = pDesc->pSocketMatrix;
	if (pDesc->szFilePath != nullptr)
		Load_From_File(pDesc->szFilePath, pDesc->pModel);

	return S_OK;
}

void CPetroch_Shell::Priority_Update(_float fTimeDelta)
{		
	__super::Priority_Update(fTimeDelta);
}

void CPetroch_Shell::Update(_float fTimeDelta)
{	
	Dessolve(fTimeDelta);
	__super::Update(fTimeDelta);
}

void CPetroch_Shell::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);	

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif // _DEBUG	

	Reposition_Shell();

	__super::Late_Update(fTimeDelta);
}

HRESULT CPetroch_Shell::Render()
{
	if (!m_isActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

	if (m_bIsDessolve) {
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRate", &m_fDissolveRate, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveDarknessRate", &m_fDissolveDarknessRate, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fSwellAmount", &m_fSwellAmount, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pNoiseTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_NoiseTexture", 4)))
			return E_FAIL;
	}

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
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
		if (m_bIsDessolve)
			iShaderPass = (_uint)SHADER_PASS_MESH::DISSOLVE;

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

	return S_OK;
}

void CPetroch_Shell::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr && static_cast<CBoss_Petroch*>(m_pOwner)->Is_Roll())
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);

	if (iGroup == (_uint)COLLISION_GROUP::PLAYER_PROJECTILE || iGroup == (_uint)COLLISION_GROUP::PLAYER_SKILL || iGroup == (_uint)COLLISION_GROUP::PLAYER_WEAPON || iGroup == (_uint)COLLISION_GROUP::SWING_HIT)
	{ 
		static_cast<CBoss_Petroch*>(m_pOwner)->Set_Block(true);
		CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Chop1.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.7f);
	}
}

void CPetroch_Shell::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CPetroch_Shell::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CPetroch_Shell::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

void CPetroch_Shell::Reposition_Shell()
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

HRESULT CPetroch_Shell::Load_From_File(const _char* szFilePath, CModel* pModel)
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

	_float3 vScale{};
	_float3 vRotation{};
	_float3 vTranslation{};

	bResult = ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vTranslation, sizeof(_float3), &dwByte, nullptr);

	CloseHandle(hFile);

	if (!strcmp(szSocketBone, "NoSocket"))
	{
		_float4x4 matSocket{};
		XMStoreFloat4x4(&matSocket, XMMatrixIdentity());

		m_pSocketMatrix = &matSocket;
	}
	else
		m_pSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name(szSocketBone);

	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Rotate(vRotation.x, vRotation.y, vRotation.z);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vTranslation));

	return S_OK;
}

HRESULT CPetroch_Shell::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EnemyDessolveNoise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;
	
	// Com_Model
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Petroch_Shell",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// Com_Shader
	if (FAILED(super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = static_cast<_uint>(COLLISION_GROUP::MONSTER_HIT);

		Desc.fRadius = 2.f;
		Desc.vCenter = { 0.f, Desc.fRadius * 0.5f , 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);		
	}

	return S_OK;
}


HRESULT CPetroch_Shell::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}


void CPetroch_Shell::Dessolve(_float fTimeDelta)
{
	if (!m_bIsDessolve)
		return;

	m_fDissolveRate -= 0.015f;
	m_fSacleRate += 0.003f;
	m_fDissolveDarknessRate += 0.05f;

	m_pTransformCom->Set_Scale(m_fSacleRate, m_fSacleRate, m_fSacleRate);

	if (m_fDissolveRate < 1.8f && m_fDissolveRate > 0.5f)
		m_fSwellAmount += 0.008f;
}

CPetroch_Shell* CPetroch_Shell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPetroch_Shell* pInstance = new CPetroch_Shell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPetroch_Shell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPetroch_Shell::Clone(void* pArg)
{
	CPetroch_Shell* pInstance = new CPetroch_Shell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CPetroch_Shell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_Shell::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);	
	Safe_Release(m_pMovementCom);
	Safe_Release(m_pNoiseTextureCom);
}
