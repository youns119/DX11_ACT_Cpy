#include "pch.h"
#include "Petroch_SharpCrystal.h"
#include "GameInstance.h"

#include "IDamaged.h"

CPetroch_SharpCrystal::CPetroch_SharpCrystal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPetroch_SharpCrystal::CPetroch_SharpCrystal(const CPetroch_SharpCrystal& Prototype)
	: super(Prototype)
{
}

HRESULT CPetroch_SharpCrystal::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPetroch_SharpCrystal::Initialize(void* pArg)
{
	CPetroch_SharpCrystal::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;	

	if (FAILED(Ready_Sounds()))
		return E_FAIL;	
	
	return S_OK;
}	

void CPetroch_SharpCrystal::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPetroch_SharpCrystal::Update(_float fTimeDelta)
{				
	if (!m_isActive)
		return;

	Decrease_LiveTime(fTimeDelta);	
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	__super::Update(fTimeDelta);
}

void CPetroch_SharpCrystal::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

#ifdef _DEBUG		
		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);		
#endif // _DEBUG


	__super::Late_Update(fTimeDelta);
}

HRESULT CPetroch_SharpCrystal::Render()
{
	if (!m_isActive)
		return S_OK;

	//if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
	//	return E_FAIL;

	//for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	//{
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
	//		return E_FAIL;

	//	m_pShaderCom->Begin(0);
	//	m_pModelCom->Render(i);
	//}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		_uint iMaterialIndex = m_pModelCom->Get_MaterialIndex_In_Mesh(i);
		_uint iShaderPass = m_pModelCom->Get_ShaderPass(iMaterialIndex);

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
	return S_OK;

	return S_OK;
}

HRESULT CPetroch_SharpCrystal::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}


HRESULT CPetroch_SharpCrystal::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{

	/*	if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;*/

		m_pShaderCom->Begin((_uint)SHADER_PASS_MESH::SHADOW);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CPetroch_SharpCrystal::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{	
	if (!m_isActive)
		return;

	IDamaged* pDamaged = dynamic_cast<IDamaged*>(pOther);
	if (pDamaged != nullptr)
		pDamaged->Damaged(10.f, IDamaged::DAMAGE::DAMAGE_NORMAL, this);
}

void CPetroch_SharpCrystal::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CPetroch_SharpCrystal::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CPetroch_SharpCrystal::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxMesh",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	CModel::DESC tModelDesc{};
	tModelDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Petroch_Crystal",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;	


	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;		
		Desc.fRadius = 2.5f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}

	return S_OK;
}

HRESULT CPetroch_SharpCrystal::Ready_Sounds()
{
	m_MaxGrowSounds.push_back(L"Petroch_CrystalGrow1.wav");
	m_MaxGrowSounds.push_back(L"Petroch_CrystalGrow2.wav");
	m_MaxGrowSounds.push_back(L"Petroch_CrystalGrow3.wav");

	m_BurstSounds.push_back(L"Petroch_CrystalBurst1.wav");
	m_BurstSounds.push_back(L"Petroch_CrystalBurst2.wav");
	m_BurstSounds.push_back(L"Petroch_CrystalBurst3.wav");	

	m_GrowSounds.push_back(L"Petroch_CrystalSpawn1.wav");
	m_GrowSounds.push_back(L"Petroch_CrystalSpawn2.wav");
	m_GrowSounds.push_back(L"Petroch_CrystalSpawn3.wav");
	m_GrowSounds.push_back(L"Petroch_CrystalSpawn4.wav");

	return S_OK;
}

void CPetroch_SharpCrystal::Spawn(_float4x4 SpawMat)
{
	m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE::RIGHT, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat4x4(&SpawMat).r[(_uint)CTransform::STATE::POSITION]);

	m_fCurLiveTime = 0.f;
	m_fScaleInterpole = 0.f;	
	m_fAdditionalLiveTime = CUtils::Compute_Random(0.f, 0.5f);	

	m_isActive = true;
	m_bGrowMax = false;
	m_bGrowOnce = false;
	m_bExplodeOnce = false;
	m_pColliderCom->Set_Active(false);
	m_pTransformCom->Set_Scale(.2f, .2f, .2f);

	_vector vPos{m_pTransformCom->Get_State(CTransform::STATE::POSITION)}, vZeroLook{XMVectorSet(0.f, 0.f, 1.f, 0.f)};
	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INSALL_SMOKE, vPos, vZeroLook, false, nullptr);
	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INSALL_SMOKEFLOOR, vPos, vZeroLook, false, nullptr);
}

void CPetroch_SharpCrystal::Play_GrowSFX()
{
	_int iIndex = CUtils::Compute_Random(0, m_GrowSounds.size() - 1);		
	CGameInstance::GetInstance()->Play_Sound(m_GrowSounds[iIndex], (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE1, .9f);
}

void CPetroch_SharpCrystal::Play_MaxGrowSFX()
{
	_int iIndex = CUtils::Compute_Random(0, m_MaxGrowSounds.size() - 1);
	CGameInstance::GetInstance()->Play_Sound(m_MaxGrowSounds[iIndex], (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, .9f);
}

void CPetroch_SharpCrystal::Play_BurstSFX()
{ 
	_int iIndex = CUtils::Compute_Random(0, m_BurstSounds.size() - 1);			
	CGameInstance::GetInstance()->Play_Sound(m_BurstSounds[iIndex], (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, .9f);
}

void CPetroch_SharpCrystal::Decrease_LiveTime(_float fTimeDelta)
{	
	if (m_fCurLiveTime >= m_fTempGrowTime && m_fCurScale < m_fMaxScale) {
		if (m_fCurLiveTime >= m_fMaxGrowTime) {	
			if (false == m_bGrowMax)
			{
				m_bGrowMax = true;
				if (m_bPlaySound) Play_MaxGrowSFX();
				_vector vPos{ m_pTransformCom->Get_State(CTransform::STATE::POSITION) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
				m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INCREASE_MAX_ELECTRIC, vPos, vZeroLook, false, nullptr);
			}
			m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
		}
		else {
			m_fScaleInterpole += fTimeDelta * 1.2f;
			if (m_fScaleInterpole < 0.0f)
				m_fScaleInterpole = 0.0f;
			if (m_fScaleInterpole > 0.22f)
				m_fScaleInterpole = 0.2f;
			if (false == m_bGrowOnce)
			{
				m_bGrowOnce = true;		
				if (m_bPlaySound) Play_GrowSFX();
				_vector vPos{ m_pTransformCom->Get_State(CTransform::STATE::POSITION) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
				m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INCREASE_ELECTRIC, vPos, vZeroLook, false, nullptr);
			}

			_float fScale = m_fCurScale + (m_fMaxScale - m_fCurScale) * m_fScaleInterpole;
			m_pTransformCom->Set_Scale(fScale, fScale, fScale);
		}
	}

	if (m_fCurLiveTime >= (m_fMaxExplodeTime + m_fAdditionalLiveTime)) {
		m_pColliderCom->Set_Active(true);
		if (false == m_bExplodeOnce)
		{
			m_bExplodeOnce = true;
			if (m_bPlaySound) Play_BurstSFX();
			_vector vPos{ m_pTransformCom->Get_State(CTransform::STATE::POSITION) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
			m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_EXPLODE_DIAMOND, vPos, vZeroLook, false, nullptr);
			m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_MESHINST_CRYSTAL_EXPLODE_0, vPos, vZeroLook, false, nullptr);
			m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_MESHINST_CRYSTAL_EXPLODE_1, vPos, vZeroLook, false, nullptr);
			m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_EXPLODE_ASH, vPos, vZeroLook, false, nullptr);
		}
	}

	if (m_fCurLiveTime >= (m_fMaxLiveTime + m_fAdditionalLiveTime)) {		
		m_pColliderCom->Set_Active(false);
		m_isActive = false;
	}	

	m_fCurLiveTime += fTimeDelta;
}

CPetroch_SharpCrystal* CPetroch_SharpCrystal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPetroch_SharpCrystal* pInstance = new CPetroch_SharpCrystal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPetroch_SharpCrystal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPetroch_SharpCrystal::Clone(void* pArg)
{
	CPetroch_SharpCrystal* pInstance = new CPetroch_SharpCrystal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CPetroch_SharpCrystal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_SharpCrystal::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}