#include "pch.h"
#include "Enemy_Frogfish.h"
#include "GameInstance.h"

//PartObject
//#include "Sardine_Horn.h"
//#include "Sardine_Tail.h"
//#include "UI_Monster.h"

////Events
//#include "Sardine_Event_Move.h"
//#include "Enemy_Event_Collider.h"

//State
//#include "Sardine_StateMachine.h"


CEnemy_Frogfish::CEnemy_Frogfish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEnemy_Frogfish::CEnemy_Frogfish(const CEnemy_Frogfish& Prototype)
	: super(Prototype)
{
}

HRESULT CEnemy_Frogfish::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Frogfish::Initialize(void* pArg)
{
	CEnemy_Frogfish::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Physics()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	if (FAILED(Ready_Events()))
		return E_FAIL;
	
	m_szName = L"Sardine";
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModelCom->SetUp_Animation(0, true);
	//m_pFSMCom->Call_NextState(L"Wait");

	return S_OK;
}	

void CEnemy_Frogfish::Priority_Update(_float fTimeDelta)
{		
	//m_pMovementCom->Priority_Update(fTimeDelta);
	//m_pFSMCom->Priority_Update(this, fTimeDelta);
	
	//if (m_pGameInstance->Key_Up(KEY::F6)) {
	//	m_iAnimIndex++;
	//	m_pModelCom->SetUp_Animation(m_iAnimIndex, true);
	//}

	__super::Priority_Update(fTimeDelta);
}

void CEnemy_Frogfish::Update(_float fTimeDelta)
{				
	m_pModelCom->Play_Animation(fTimeDelta);
	//m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	//if (!m_bIsAggroed) {
	//	m_bIsAggroed = Detect_Target(90.f, 15.f, 8.f, m_pPlayerTransform, m_pTransformCom);
	//	if (m_bIsAggroed)
	//		m_pFSMCom->Call_NextState(L"Aggroed");
	//}
	//	
	//// Movement
	//{
	//	_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	//		
	//	m_pFSMCom->Update(this, fTimeDelta);
	//	Chase(fTimeDelta);
	//	m_pMovementCom->Update(fTimeDelta);

	//	_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	//	_vec3 vDisp = vCurPosition - vPrePosition;
	//	if (m_pPhysicsControllerCom != nullptr)
	//		m_pPhysicsControllerCom->Move(vDisp, static_cast<_float>(fTimeDelta * 0.5), fTimeDelta);
	//}

	__super::Update(fTimeDelta);
}

void CEnemy_Frogfish::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
//	if (nullptr != m_pPhysicsControllerCom) {
//		m_pGameInstance->Simulate_Physics(m_pPhysicsControllerCom);
//
//#ifdef _DEBUG
//		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
//		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsControllerCom);
//#endif // _DEBUG
//	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CEnemy_Frogfish::Render()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	if (m_bIsDessolve) {
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRate", &m_fDissolveRate, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveDarknessRate", &m_fDissolveDarknessRate, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fSwellAmount", &m_fSwellAmount, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pNoiseTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_NoiseTexture", 2)))
			return E_FAIL;
	}

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		_uint iShaderPass = 0;
		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::DEFAULT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR;

			if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
				return E_FAIL;
		}
		if (m_bIsDessolve)
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::DISSOLVE;

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

HRESULT CEnemy_Frogfish::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)SHADER_PASS_ANIMMESH::SHADOW);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CEnemy_Frogfish::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{			
}

void CEnemy_Frogfish::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy_Frogfish::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CEnemy_Frogfish::Ready_Components()
{
	/* Com_NoiseTexture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EnemyDessolveNoise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;

	/* Com_Shader */
 	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	CModel::DESC tModelDesc{};
	tModelDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Enemy_Frogfish",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;	

	/* Com_FSM */
	{
		CFSM::FSM_DESC Desc{};
		Desc.eMode = CFSM::FSM_PRIOR;
		Desc.pOwner = this;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Sardine_StateMachine",
			L"Com_FSM", reinterpret_cast<CComponent**>(&m_pFSMCom), &Desc)))
			return E_FAIL;
	}

	if (m_pMovementCom == nullptr)
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 50.f;
		Desc.fMoveDecel = 0.f;
		Desc.fMaxSpeed = 5.f;
		Desc.fMaxFallingSpeed = 10.f;
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
		Desc.fRadius = 1.f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);
	}

	return S_OK;
}

HRESULT CEnemy_Frogfish::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fScaleCoeff = 0.5f;
		Desc.fRadius = 1.0f;
		Desc.fHeight = 0.3f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Frogfish::Ready_PartObject()
{
	//// Horn
	//{
	//	CSardine_Horn::DESC HornDesc{};

	//	HornDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
	//	HornDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Jaw.11_end_end_end");
	//	HornDesc.pOwner = this;

	//	if (FAILED(Add_PartObject(L"Part_Horn", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Sardine_Horn", &HornDesc)))
	//		return E_FAIL;
	//}

	//// Tail
	//{
	//	CSardine_Tail::DESC TailDesc{};

	//	TailDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
	//	TailDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Tail_004.18");
	//	TailDesc.pOwner = this;

	//	if (FAILED(Add_PartObject(L"Part_Tail", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Sardine_Tail", &TailDesc)))
	//		return E_FAIL;
	//}

	//// UI
	//{				
	//	CUI_Monster::DESC MonsterDesc{};
	//	{
	//		MonsterDesc.pEnemyStats = &m_EnemyStats;
	//		MonsterDesc.pMonsterWorld = &m_pTransformCom->Get_WorldMatrix_Ref();
	//		MonsterDesc.pIsAggroed = &m_bIsAggroed;	
	//		MonsterDesc.pIsDead = &m_bIsDead;
	//	}
	//	if (FAILED(Add_PartObject(L"Part_UI_Monster", LEVEL_STATIC, L"Prototype_GameObject_UI_Monster", &MonsterDesc)))
	//		return E_FAIL;
	//}

	return S_OK;
}

void CEnemy_Frogfish::Chase(_float fTimeDelta)
{
	if (!m_bIsChase)
		return;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 2.f) {
		m_bIsChase = false;
		return;
	}

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Straight(fTimeDelta * m_fChaseSpeed);
}

void CEnemy_Frogfish::Select_Action()
{
	//if (!m_bIsAggroed) {
	//	if (m_PatrolPoints.size() > 0)
	//		m_pFSMCom->Call_NextState(L"Patrol");
	//	else
	//		m_pFSMCom->Select_StatesFromGroup((_uint)CSardine_StateMachine::STATE_UNAGGRO);
	//	return;
	//}

	//_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	//_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	//if (fDis < 4) {
	//	m_pFSMCom->Select_StatesFromGroup((_uint)CSardine_StateMachine::STATE_CLOSE);
	//}
	//else if (fDis < 8) {
	//	m_pFSMCom->Select_StatesFromGroup((_uint)CSardine_StateMachine::STATE_RANGED);
	//}
	//else if (fDis < 25) {
	//	m_pFSMCom->Select_StatesFromGroup((_uint)CSardine_StateMachine::STATE_FAR);
	//}
	//else {		
	//	m_pFSMCom->Select_StatesFromGroup((_uint)CSardine_StateMachine::STATE_OUTOFRANGE);		
	//}
}

void CEnemy_Frogfish::Parried()
{
	Reset_RecoverDelay();
	m_bIsDamaged = true;
	if (!m_EnemyStats.bIsBalanceBreakable && !m_EnemyStats.bIsCapsized) {
		m_EnemyStats.fCurBP += 10.f;

		if (m_EnemyStats.fCurBP >= m_EnemyStats.fMaxBP)
			m_EnemyStats.bIsBalanceBreakable = true;
	}

	if (m_EnemyStats.bIsBalanceBreakable /*&& It's Player's Heavy Attack*/) {
		m_pFSMCom->Call_NextState(L"Capsized");
		Reset_Balance();
		m_EnemyStats.bIsCapsized = true;
		m_EnemyStats.bIsBalanceBreakable = false;
		return;
	}

	m_pFSMCom->Call_NextState(L"Stagger");
}

void CEnemy_Frogfish::Active()
{
	if (m_bIsDead)
		return;

	Ready_Physics();

	super::Active();
}

void CEnemy_Frogfish::Deactive()
{	
	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}

	super::Deactive();
}

void CEnemy_Frogfish::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
{
	if (m_bIsDead)
		return;
	m_bIsDamaged = true;
	Reset_RecoverDelay();
	m_EnemyStats.fCurHP -= fDamage;

	if (m_EnemyStats.fCurHP <= 0) {
		m_EnemyStats.fCurHP = 0;
		m_bIsDead = true;
		m_pFSMCom->Call_NextState(L"Death");

		m_pMovementCom->Set_GravityScale(0.f);
		m_pMovementCom->Set_MaxFallingSpeed(0.f);

		_vector vForceDir = m_pPlayerTransform->Get_State(CTransform::STATE::LOOK);		
		vForceDir = XMVectorSetY(vForceDir, XMVectorGetY(vForceDir) + 5.f);
		m_pMovementCom->Add_Force(XMVector4Normalize(vForceDir), 11.5f);
		return;
	}

	if (m_EnemyStats.bIsBalanceBreakable /*&& Player's Heavy Attack*/) {
		m_pFSMCom->Call_NextState(L"Capsized");
		Reset_Balance();
		m_EnemyStats.bIsCapsized = true;		
		return;
	}

	if (!m_EnemyStats.bIsBalanceBreakable && !m_EnemyStats.bIsCapsized) {
		m_EnemyStats.fCurBP += fDamage;

		if (m_EnemyStats.fCurBP >= m_EnemyStats.fMaxBP) {
			m_EnemyStats.fCurBP = m_EnemyStats.fMaxBP;
			m_EnemyStats.bIsBalanceBreakable = true;
		}

		m_pFSMCom->Call_NextState(L"Stagger");		
	}

	CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Blood1.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.7f);
}

void CEnemy_Frogfish::Respawn()
{
	m_pMovementCom->Set_GravityScale(10.f);
	m_pMovementCom->Set_MaxFallingSpeed(10.f);

	if (FAILED(Ready_Physics()))
		return;

	_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);	
	super::Respawn();
	_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	m_pPhysicsControllerCom->Set_Position(vCurPosition);
	
	m_bIsChase = false;
	m_pColliderCom->Set_Active(true);
	m_pFSMCom->Call_NextState(L"Wait");
}

void CEnemy_Frogfish::Death()
{
	m_pColliderCom->Set_Active(false);
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);

	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}

	super::Death();
	m_pFSMCom->Clear_State();
}

HRESULT CEnemy_Frogfish::Ready_Events()
{
	//CCollider* pTailCollider = static_cast<CSardine_Tail*>(Find_PartObject({ L"Part_Horn" }))->Get_Collider();
	//CCollider* pHornCollider = static_cast<CSardine_Horn*>(Find_PartObject({ L"Part_Tail" }))->Get_Collider();

	///* 이동 이벤트 */
	//{
	//	if (FAILED(Add_Event(L"0_Spin_Move", CSardine_Event_Move::Create(this, true, 1.3f))) ||
	//		FAILED(Add_Event(L"1_Spin_Stop", CSardine_Event_Move::Create(this))) ||
	//		FAILED(Add_Event(L"0_Charge_Back", CSardine_Event_Move::Create(this, true, -1.3f))) ||
	//		FAILED(Add_Event(L"1_Charge_Forward", CSardine_Event_Move::Create(this, true, 3.0f))) ||
	//		FAILED(Add_Event(L"2_Charge_Stop", CSardine_Event_Move::Create(this))))
	//		return E_FAIL;
	//}

	///* 콜라이더 이벤트 */
	//{
	//	if (FAILED(Add_Event(L"Charge_Col_On", CEnemy_Event_Collider::Create(this, pTailCollider, true))) ||
	//		FAILED(Add_Event(L"Charge_Col_Off", CEnemy_Event_Collider::Create(this, pTailCollider, false))) ||
	//		FAILED(Add_Event(L"Spin_Col_On", CEnemy_Event_Collider::Create(this, pHornCollider, true))) ||
	//		FAILED(Add_Event(L"Spin_Col_Off", CEnemy_Event_Collider::Create(this, pHornCollider, false))))
	//		return E_FAIL;
	//}

	return S_OK;
}

CEnemy_Frogfish* CEnemy_Frogfish::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Frogfish* pInstance = new CEnemy_Frogfish(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CEnemy_Frogfish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CEnemy_Frogfish::Clone(void* pArg)
{
	CEnemy_Frogfish* pInstance = new CEnemy_Frogfish(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEnemy_Frogfish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Frogfish::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pFSMCom);	
	Safe_Release(m_pMovementCom);
	Safe_Release(m_pPhysicsControllerCom);
	Safe_Release(m_pNoiseTextureCom);
}