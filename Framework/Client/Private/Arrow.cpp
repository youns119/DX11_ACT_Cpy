#include "pch.h"
#include "Arrow.h"
#include "GameInstance.h"

#include "IDamaged.h"

#include "Effect_Colored_Trail.h"

CArrow::CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CArrow::CArrow(const CArrow& Prototype)
	: super(Prototype)
{
}

HRESULT CArrow::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CArrow::Initialize(void* pArg)
{
	CArrow::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;	

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;
	
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}	

void CArrow::Priority_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	m_pMovementCom->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CArrow::Update(_float fTimeDelta)
{				
	if (!m_bIsActive)
		return;

	m_fLifeTimeAcc += fTimeDelta;
	if (m_fLifeTime <= m_fLifeTimeAcc)
	{
		Deactive();
		return;
	}		

	if (Detect_Ground()) {
		m_pMovementCom->Reset_Velocity();
		m_pColliderCom->Set_Active(false);
		m_pMovementCom->Set_MaxFallingSpeed(0.f);
		m_pMovementCom->Set_GravityScale(0.f);
	}
	else if (m_pMovementCom->Get_HorizontalSpeed() > 65.f) {
		_vector vRotaton = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
		m_pMovementCom->Add_Velocity(XMVector4Normalize(vRotaton), fTimeDelta);		
	}		
	
	m_pMovementCom->Update(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	
	__super::Update(fTimeDelta);
}

void CArrow::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
	//m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

	m_pMovementCom->Late_Update(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);	
#endif // _DEBUG

	__super::Late_Update(fTimeDelta);
}

void CArrow::Final_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;
	__super::Final_Update(fTimeDelta);
}

HRESULT CArrow::Render()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CArrow::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_pShaderCom->Begin((_uint)SHADER_PASS_ANIMMESH::SHADOW);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CArrow::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{	
	IDamaged* pDamaged = dynamic_cast<IDamaged*>(pOther);
	if (pDamaged != nullptr) {
		pDamaged->Damaged(10.f, IDamaged::DAMAGE::DAMAGE_NORMAL, this);
		CGameInstance::GetInstance()->Play_Sound(L"Bowman Arrow Impact 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	}		
}

void CArrow::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CArrow::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CArrow::Fire_Arrow(_vector vTargetPos,_float4x4 StartMat)
{
	m_bIsActive = true;
	m_pColliderCom->Set_Active(true);
	m_pMovementCom->Set_MaxFallingSpeed(1.f);
	m_pMovementCom->Set_GravityScale(10.f);
	m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMLoadFloat4x4(&StartMat).r[(_uint)CTransform::STATE::LOOK]);
	m_pTransformCom->Set_State(CTransform::STATE::UP, XMLoadFloat4x4(&StartMat).r[(_uint)CTransform::STATE::UP]);
	m_pTransformCom->Set_State(CTransform::STATE::RIGHT, XMLoadFloat4x4(&StartMat).r[(_uint)CTransform::STATE::RIGHT]);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat4x4(&StartMat).r[(_uint)CTransform::STATE::POSITION]);

	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	vLookTarget = XMVectorSetY(vLookTarget, XMVectorGetY(vLookTarget) + 1.f);
	m_pTransformCom->LookAt(vLookTarget);

	_vector vDir{};
	vDir = vDir + (m_pTransformCom->Get_State(CTransform::STATE::LOOK));	
	m_pMovementCom->Add_Force(vDir, 80.f);

	static_cast<CEffect_Colored_Trail*>(m_PartObjects.find(L"Part_Trail")->second)->Call_Trail();
}

_bool CArrow::Detect_Ground()
{
	_float fDistance;
	_float fMaxDistance = 2.f;	
	_vec3 vOrigin = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vec3 vDirection = m_pTransformCom->Get_State(CTransform::STATE::LOOK);	

	_bool bIsHit = CGameInstance::GetInstance()->Ray_Cast_To_Physics(vOrigin, vDirection, fMaxDistance, fDistance, PxQueryFlag::eSTATIC);
	return bIsHit;
}

void CArrow::Deactive()
{
	m_bIsActive = false;
	m_fLifeTimeAcc = 0.f;
	m_pColliderCom->Set_Active(false);
	m_pMovementCom->Reset_Velocity();
	static_cast<CEffect_Colored_Trail*>(m_PartObjects.find(L"Part_Trail")->second)->Clear_Info();
}

HRESULT CArrow::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxMesh",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	CModel::DESC tModelDesc{};
	tModelDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Bowman_Arrow",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;	

  // Com_Movement
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 150.f;
		Desc.fMoveDecel = 5.f;
		Desc.fMaxSpeed = 50.f;
		Desc.fMaxFallingSpeed = .1f;
		Desc.fRotationScale = 1;
		Desc.fGravityScale = 10.f;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
			L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &Desc)))
			return E_FAIL;

		m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
		m_pMovementCom->Set_RotateToDirection(false);
	}

	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;		
		Desc.fRadius = 0.7f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}

	return S_OK;
}

HRESULT CArrow::Ready_PartObjects()
{
	CGameObject::GAMEOBJECT_DESC PartDesc{};
	PartDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
	if (FAILED(Add_PartObject(L"Part_Trail", LEVEL_STATIC, L"Prototype_GameObject_Effect_ColoredTrail", &PartDesc)))
		return E_FAIL;

	return S_OK;
}

CArrow* CArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArrow* pInstance = new CArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CArrow::Clone(void* pArg)
{
	CArrow* pInstance = new CArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArrow::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);	
	Safe_Release(m_pMovementCom);	
}

