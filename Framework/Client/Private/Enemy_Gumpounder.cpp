#include "pch.h"
#include "Enemy_Gumpounder.h"
#include "GameInstance.h"

//IK
#include "IK_Manager.h"
#include "IK_Segment.h"
#include "Bone.h"

//PartObject
#include "Gumpounder_Hammer.h"
#include "UI_Monster.h"

//Events
#include "Enemy_Event_Collider.h"
#include "Enemy_Event_Sound.h"
//#include "Sardine_Event_Move.h"
//#include "Sardine_Event_Effect.h"

//State
#include "Gumpounder_StateMachine.h"




CEnemy_Gumpounder::CEnemy_Gumpounder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEnemy_Gumpounder::CEnemy_Gumpounder(const CEnemy_Gumpounder& Prototype)
	: super(Prototype)
{
}

HRESULT CEnemy_Gumpounder::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Gumpounder::Initialize(void* pArg)
{
	CEnemy_Gumpounder::DESC* pDesc = static_cast<DESC*>(pArg);

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

	if (FAILED(Ready_IK()))
		return E_FAIL;
		
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pFSMCom->Call_NextState(L"Wait");

	//m_pModelCom->SetUp_Animation(1, true);
	m_pIKManager->Set_Operation(false);
	return S_OK;
}	

void CEnemy_Gumpounder::Priority_Update(_float fTimeDelta)
{		
	m_pMovementCom->Priority_Update(fTimeDelta);
	m_pFSMCom->Priority_Update(this, fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CEnemy_Gumpounder::Update(_float fTimeDelta)
{				
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsAggroed) {
		m_bIsAggroed = Detect_Target(90.f, 15.f, 8.f, m_pPlayerTransform, m_pTransformCom);
		if (m_bIsAggroed)
			m_pFSMCom->Call_NextState(L"Aggroed");
	}
		
	// Movement
	{
		_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
			
		m_pFSMCom->Update(this, fTimeDelta);
		Chase(fTimeDelta);
		m_pMovementCom->Update(fTimeDelta);

		if (m_pGameInstance->Key_Press(KEY::UP)) {
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (m_pGameInstance->Key_Press(KEY::DOWN)) {
			m_pTransformCom->Go_Backward(fTimeDelta);
		}
		if (m_pGameInstance->Key_Press(KEY::LEFT)) {
			m_pTransformCom->Turn(XMVectorSet(0.f, -1.f, 0.f, 0.f), fTimeDelta);
		}
		if (m_pGameInstance->Key_Press(KEY::RIGHT)) {
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}

		if (m_pGameInstance->Key_Press(KEY::H)) {
			//m_pIKManager->Reset_Position();
			m_pIKManager->Set_Operation(true);
		}

		_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vec3 vDisp = vCurPosition - vPrePosition;
		if (m_pPhysicsControllerCom != nullptr)
			m_pPhysicsControllerCom->Move(vDisp, static_cast<_float>(fTimeDelta * 0.5), fTimeDelta);
	}

	__super::Update(fTimeDelta);
}

void CEnemy_Gumpounder::Late_Update(_float fTimeDelta)
{
	m_pIKManager->Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
	if (nullptr != m_pPhysicsControllerCom) {
		m_pGameInstance->Simulate_Physics(m_pPhysicsControllerCom);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsControllerCom);
#endif // _DEBUG
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CEnemy_Gumpounder::Render()
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
		else if (m_bIsDamaged)
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::ENEMYDAMAGED;

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

HRESULT CEnemy_Gumpounder::Render_Shadow()
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

void CEnemy_Gumpounder::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{	
}

void CEnemy_Gumpounder::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy_Gumpounder::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CEnemy_Gumpounder::Ready_Components()
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
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Enemy_Gumpounder",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;	

	/* Com_FSM */
	{
		CFSM::FSM_DESC Desc{};
		Desc.eMode = CFSM::FSM_RAND;
		Desc.pOwner = this;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Gumpounder_StateMachine",
			L"Com_StateMachine", reinterpret_cast<CComponent**>(&m_pFSMCom), &Desc)))
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
		Desc.fRadius = 2.f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);
	}

	return S_OK;
}

HRESULT CEnemy_Gumpounder::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());		
		Desc.fRadius = 2.0f;
		Desc.fHeight = 0.3f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Gumpounder::Ready_PartObject()
{
	// Hammer
	{
		CGumpounder_Hammer::DESC HammerDesc{};

		HammerDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		HammerDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("HammerHead");
		HammerDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_Hammer", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Gumpounder_Hammer", &HammerDesc)))
			return E_FAIL;
	}

	// UI
	{				
		CUI_Monster::DESC MonsterDesc{};
		{
			MonsterDesc.pEnemyStats = &m_EnemyStats;
			MonsterDesc.pMonsterWorld = &m_pTransformCom->Get_WorldMatrix_Ref();

			MonsterDesc.pIsAggroed = &m_bIsAggroed;
			MonsterDesc.pIsDead = &m_bIsDead;

		}
		if (FAILED(Add_PartObject(L"Part_UI_Monster", LEVEL_STATIC, L"Prototype_GameObject_UI_Monster", &MonsterDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CEnemy_Gumpounder::Chase(_float fTimeDelta)
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

void CEnemy_Gumpounder::Select_Action()
{
	//if (!m_bIsAggroed) {
	//	if (m_PatrolPoints.size() > 0)
	//		m_pFSMCom->Call_NextState(L"Patrol");
	//	else
	//		m_pFSMCom->Select_StatesFromGroup((_uint)CGumpounder_StateMachine::STATE_UNAGGRO);
	//	return;
	//}

	//_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	//_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	//if (fDis < 8) {
	//	m_pFSMCom->Select_StatesFromGroup((_uint)CGumpounder_StateMachine::STATE_CLOSE);
	//}	
	//else {		
	//	m_pFSMCom->Select_StatesFromGroup((_uint)CGumpounder_StateMachine::STATE_RANGED);
	//}
}

void CEnemy_Gumpounder::Parried()
{
	Reset_RecoverDelay();
	m_bIsDamaged = true;	

	if (!m_EnemyStats.bIsBalanceBreakable && !m_EnemyStats.bIsCapsized) {
		m_EnemyStats.fCurBP += 10.f;

		if (m_EnemyStats.fCurBP >= m_EnemyStats.fMaxBP)
			m_EnemyStats.bIsBalanceBreakable = true;
	}

	if (m_EnemyStats.bIsBalanceBreakable) {
		m_pFSMCom->Call_NextState(L"Capsized");
		Reset_Balance();
		m_EnemyStats.bIsCapsized = true;
		m_EnemyStats.bIsBalanceBreakable = false;
		return;
	}

	m_pFSMCom->Call_NextState(L"Stagger");
}

void CEnemy_Gumpounder::Active()
{	
	Ready_Physics();

	super::Active();
	m_pIKManager->Reset_Position();
}

void CEnemy_Gumpounder::Deactive()
{	
	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}

	super::Deactive();
}

void CEnemy_Gumpounder::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
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
		return;
	}

	if (m_EnemyStats.bIsBalanceBreakable && eDamage == DAMAGE::DAMAGE_HEAVY) {
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

	CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Sword3.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.7f);
}

void CEnemy_Gumpounder::Respawn()
{
	m_pIKManager->Set_Operation(true);
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

void CEnemy_Gumpounder::Death()
{
	super::Death();

	m_pColliderCom->Set_Active(false);
	m_pFSMCom->Clear_State();
}

HRESULT CEnemy_Gumpounder::Ready_Events()
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

	///* 사운드 이벤트 */
	//{
	//	if (FAILED(Add_Event(L"Sound_SpinAttack", CEnemy_Event_Sound::Create(this, L"Sardine Attack Impact 3.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||			
	//		FAILED(Add_Event(L"Sound_TailAttack", CEnemy_Event_Sound::Create(this, L"Sardine Attack VO 2.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK2))))
	//		return E_FAIL;
	//}

	//{
	//	if (FAILED(Add_Event(L"Effect_Sardine_Distortion", CSardine_Event_Effect::Create(this))))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CEnemy_Gumpounder::Ready_IK()
{
	CIK_Manager::IK_DESC IKDesc{};
	IKDesc.pOwner = this;
	IKDesc.iSegmentGroupSize = 6;
	IKDesc.fConstraintAngle = 50.f;
	IKDesc.fConstraintMaxDistance = 3.f;
	IKDesc.fConstraintMinDistance = 0.5f;	
	//IKDesc.fDefaultDistance = 1.5f;
	IKDesc.pRootBoneTransform = m_pTransformCom;
	IKDesc.fLerpSpeed = 2.5f;
	IKDesc.fMaxHeight = 0.08f;

	m_pIKManager = CIK_Manager::Create(&IKDesc);

	_int iGroupIndex = 0;
	{
		Set_Segment("Thigh.L.1", -80.f, 1.5f, iGroupIndex);
		Set_Segment("Knee_L_1", -80.f, 1.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh.L.2", -110.f, 1.5f, iGroupIndex);
		Set_Segment("Knee_L_2", -110.f, 1.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh.L.3", -140.f, 1.5f, iGroupIndex);
		Set_Segment("Knee_L_3", -140.f, 1.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh.R.1", 80.f, 1.5f, iGroupIndex);
		Set_Segment("Knee_R_1", 80.f, 1.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh.R.2", 110.f, 1.5f, iGroupIndex);
		Set_Segment("Knee_R_2", 110.f, 1.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh.R.3", 140.f, 1.5f, iGroupIndex);
		Set_Segment("Knee_R_3", 140.f, 1.5f, iGroupIndex);
	}

	return S_OK;
}

void CEnemy_Gumpounder::Set_Segment(const _char* szBoneName, _float fBaseAngle, _float fDefaultDistance, _int _iGroupIndex)
{
	CIK_Segment::IK_Segment_DESC SegmentDesc{};
	SegmentDesc.pOwner = this;
	SegmentDesc.fBaseAngle = fBaseAngle;
	SegmentDesc.fPitchOffset = 89.f;
	SegmentDesc.fDefaultDistance = fDefaultDistance;
	SegmentDesc.pCombinedMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name(szBoneName);		

	m_pIKManager->Add_SegmentGroup(_iGroupIndex, CIK_Segment::Create(&SegmentDesc));
}


CEnemy_Gumpounder* CEnemy_Gumpounder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Gumpounder* pInstance = new CEnemy_Gumpounder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CEnemy_Gumpounder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CEnemy_Gumpounder::Clone(void* pArg)
{
	CEnemy_Gumpounder* pInstance = new CEnemy_Gumpounder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEnemy_Gumpounder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Gumpounder::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pFSMCom);	
	Safe_Release(m_pMovementCom);
	Safe_Release(m_pPhysicsControllerCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pIKManager);
}