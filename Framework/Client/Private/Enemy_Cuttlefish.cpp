#include "pch.h"
#include "Enemy_Cuttlefish.h"
#include "GameInstance.h"

//PartObject
#include "Cuttlefish_Fist.h"
#include "Cuttlefish_GrabTentacle.h"
#include "Cuttlefish_Tail.h"
#include "UI_Monster.h"
#include "Cuttlefish_Missile.h"
#include "CineCamera.h"

//Events
#include "Enemy_Event_Collider.h"
#include "Enemy_Event_Sound.h"
#include "Cuttlefish_Event_Move.h"
#include "Cuttlefish_Event_MoveSide.h"
#include "Cuttlefish_Event_MissileL.h"
#include "Cuttlefish_Event_MissileR.h"
#include "Cuttlefish_Event_GrabOff.h"
#include "Cuttlefish_Event_ExecutionDamage.h"
#include "Cuttlefish_Event_Effect.h"

//State
#include "Cuttlefish_StateMachine.h"

CEnemy_Cuttlefish::CEnemy_Cuttlefish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEnemy_Cuttlefish::CEnemy_Cuttlefish(const CEnemy_Cuttlefish& Prototype)
	: super(Prototype)
{
}

HRESULT CEnemy_Cuttlefish::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Cuttlefish::Initialize(void* pArg)
{
	CEnemy_Cuttlefish::DESC* pDesc = static_cast<DESC*>(pArg);

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

	if (FAILED(Ready_Missile()))
		return E_FAIL;
	
	m_szName = L"Cuttlefish";
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));	
	m_pFSMCom->Call_NextState(L"Wait");

	return S_OK;
}	

void CEnemy_Cuttlefish::Priority_Update(_float fTimeDelta)
{		
	m_pMovementCom->Priority_Update(fTimeDelta);
	m_pFSMCom->Priority_Update(this, fTimeDelta);

	for (auto& Missile : m_Missiles)
		Missile->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CEnemy_Cuttlefish::Update(_float fTimeDelta)
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
		MoveSide(fTimeDelta);
		m_pMovementCom->Update(fTimeDelta);

		_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vec3 vDisp = vCurPosition - vPrePosition;
		if (m_pPhysicsControllerCom != nullptr)
			m_pPhysicsControllerCom->Move(vDisp, static_cast<_float>(fTimeDelta * 0.5), fTimeDelta);
	}

	for (auto& Missile : m_Missiles)
		Missile->Update(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CEnemy_Cuttlefish::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
	if (nullptr != m_pPhysicsControllerCom) {
		m_pGameInstance->Simulate_Physics(m_pPhysicsControllerCom);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsControllerCom);
#endif // _DEBUG
	}

	for (auto& Missile : m_Missiles)
		Missile->Late_Update(fTimeDelta);

	__super::Late_Update(fTimeDelta);
}	

HRESULT CEnemy_Cuttlefish::Render()
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


	for (auto& Missile : m_Missiles)
		Missile->Render();

	return S_OK;
}

HRESULT CEnemy_Cuttlefish::Render_Shadow()
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

void CEnemy_Cuttlefish::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{			
}

void CEnemy_Cuttlefish::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy_Cuttlefish::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CEnemy_Cuttlefish::Ready_Components()
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
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Enemy_Cuttlefish",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;	

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

	/* Com_FSM */
	{
		CFSM::FSM_DESC Desc{};
		Desc.eMode = CFSM::FSM_RAND;
		Desc.pOwner = this;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Cuttlefish_StateMachine",
			L"Com_StateMachine", reinterpret_cast<CComponent**>(&m_pFSMCom), &Desc)))
			return E_FAIL;

		m_pFSMCom->Refresh_AllStates();
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

HRESULT CEnemy_Cuttlefish::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fScaleCoeff = 0.5f;
		Desc.fRadius = 0.5f;
		Desc.fHeight = 0.5f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Cuttlefish::Ready_PartObject()
{

	// arm_end_L.13 : 그랩
	// body_04_x.21 : 꼬리
	// tentacle2_03_R.43_end : 펀치
	
	// Fist
	{
		CCuttlefish_Fist::DESC HornDesc{};

		HornDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		HornDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("tentacle0_00_X.24");
		HornDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_Fist", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Cuttlefish_Fist", &HornDesc)))
			return E_FAIL;
	}

	// Tail
	{
		CCuttlefish_Tail::DESC TailDesc{};

		TailDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		TailDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("body_04_x.21");
		TailDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_Tail", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Cuttlefish_Tail", &TailDesc)))
			return E_FAIL;
	}

	// Grab
	{
		CCuttlefish_GrabTentacle::DESC TailDesc{};

		TailDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		TailDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("arm_end_L.13");
		TailDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_Grab", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Cuttlefish_GrabTentacle", &TailDesc)))
			return E_FAIL;
	}

	// UI
	{
		CUI_Monster::DESC MonsterDesc{};
		{
			MonsterDesc.pEnemyStats = &m_EnemyStats;
			MonsterDesc.pMonsterWorld = &m_pTransformCom->Get_WorldMatrix_Ref();
			MonsterDesc.pIsDead = &m_bIsDead;
			MonsterDesc.pIsAggroed = &m_bIsAggroed;
		}
		if (FAILED(Add_PartObject(L"Part_UI_Monster", LEVEL_STATIC, L"Prototype_GameObject_UI_Monster", &MonsterDesc)))
			return E_FAIL;
	}

	// CineCamera
	{
		CCineCamera::DESC CineCameraDesc{};

		CineCameraDesc.CameraDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
		CineCameraDesc.CameraDesc.fNear = 0.1f;
		CineCameraDesc.CameraDesc.fFar = 1000.f;

		CineCameraDesc.strName = to_wstring(m_iID);

		if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
			return E_FAIL;

		m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
		m_pCineCamera->Add_Cinematic(L"Execute", "../Bin/DataFiles/CameraData/Cuttlefish_Execute.data");
	}

	return S_OK;
}


HRESULT CEnemy_Cuttlefish::Ready_Missile()
{
	for (_int i = 0; i < 2; i++)
	{
		CCuttlefish_Missile::DESC CrystalSpawnerDesc{};
		CrystalSpawnerDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
		CrystalSpawnerDesc.pOwner = this;
		CCuttlefish_Missile* pCrystalSpawner = static_cast<CCuttlefish_Missile*>(m_pGameInstance->Clone_Prototype(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Cuttlefish_Missile", &CrystalSpawnerDesc));

		pCrystalSpawner->Set_ParentWorldMatrix(&m_pTransformCom->Get_WorldMatrix_Ref());
		pCrystalSpawner->Set_IsActive(false);

		m_Missiles.push_back(pCrystalSpawner);
	}

	return S_OK;
}

void CEnemy_Cuttlefish::Chase(_float fTimeDelta)
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

void CEnemy_Cuttlefish::MoveSide(_float fTimeDelta)
{
	if (!m_bIsSideMove)
		return;

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Right(fTimeDelta * m_fSideMoveSpeed);
}

void CEnemy_Cuttlefish::Locate(_vec3 vTargetPos)
{
	if (nullptr != m_pPhysicsControllerCom)
		m_pPhysicsControllerCom->Set_Position(vTargetPos);
}

void CEnemy_Cuttlefish::Select_Action()
{
	if (!m_bIsAggroed) {
			m_pFSMCom->Select_StatesFromGroup((_uint)CCuttlefish_StateMachine::STATE_UNAGGRO);
		return;
	}

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 12) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CCuttlefish_StateMachine::STATE_CLOSE);
	}
	else if (fDis < 18) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CCuttlefish_StateMachine::STATE_RANGED);
	}
	else if (fDis <  30) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CCuttlefish_StateMachine::STATE_FAR);
	}
	else {						
		m_pFSMCom->Call_NextState(L"Wait");
		m_bIsAggroed = false;
	}
}

void CEnemy_Cuttlefish::Parried()
{
	Reset_RecoverDelay();

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

void CEnemy_Cuttlefish::Active()
{
	if (m_bIsDead)
		return;

	Ready_Physics();

	super::Active();
}

void CEnemy_Cuttlefish::Deactive()
{	
	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}

	super::Deactive();
}

void CEnemy_Cuttlefish::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
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

	if (!m_EnemyStats.bIsBalanceBreakable && !m_EnemyStats.bIsCapsized) {
		m_EnemyStats.fCurBP += fDamage;

		if (m_EnemyStats.fCurBP >= m_EnemyStats.fMaxBP) {
			m_EnemyStats.fCurBP = m_EnemyStats.fMaxBP;
			m_EnemyStats.bIsBalanceBreakable = true;
		}
	}

	if (m_EnemyStats.bIsBalanceBreakable && eDamage == DAMAGE::DAMAGE_HEAVY) {
		m_pFSMCom->Call_NextState(L"Capsized");
		Reset_Balance();
		m_EnemyStats.bIsCapsized = true;
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Sword3.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.7f);
}

void CEnemy_Cuttlefish::Respawn()
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

void CEnemy_Cuttlefish::Death()
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

HRESULT CEnemy_Cuttlefish::Ready_Events()
{	
	CCollider* pTailCollider = static_cast<CCuttlefish_Tail*>(Find_PartObject({ L"Part_Tail" }))->Get_Collider();
	CCollider* pFistCollider = static_cast<CCuttlefish_Fist*>(Find_PartObject({ L"Part_Fist" }))->Get_Collider();
	CCollider* pGrabCollider = static_cast<CCuttlefish_GrabTentacle*>(Find_PartObject({ L"Part_Grab" }))->Get_Collider();

	/* 이동 이벤트 */
	{
		if (FAILED(Add_Event(L"Move_Look", CCuttlefish_Event_Move::Create(this, true, 0.f))) ||
			FAILED(Add_Event(L"Move_Forward", CCuttlefish_Event_Move::Create(this, true, 5.0f))) ||
			FAILED(Add_Event(L"Grab_Move_Forward", CCuttlefish_Event_Move::Create(this, true, 4.0f))) ||
			FAILED(Add_Event(L"Grab_Move_Back", CCuttlefish_Event_Move::Create(this, true, -4.0f))) ||
			FAILED(Add_Event(L"Move_Left", CCuttlefish_Event_MoveSide::Create(this, true, 1.2f))) ||
			FAILED(Add_Event(L"Move_Right", CCuttlefish_Event_MoveSide::Create(this, true, -1.2f))) ||
			FAILED(Add_Event(L"SideMove_Stop", CCuttlefish_Event_MoveSide::Create(this, false))) ||
			FAILED(Add_Event(L"Move_Stop", CCuttlefish_Event_Move::Create(this, false))))
			return E_FAIL;
	}

		/* 콜라이더 이벤트 */
	{
		if (FAILED(Add_Event(L"Throw_Projectile_L", CCuttlefish_Event_MissileL::Create(this))) ||
			FAILED(Add_Event(L"Throw_Projectile_R", CCuttlefish_Event_MissileR::Create(this))) ||
			FAILED(Add_Event(L"Grab_Off", CCuttlefish_Event_GrabOff::Create(this))) ||
			FAILED(Add_Event(L"Execution_Damage", CCuttlefish_Event_ExecutionDamage::Create(this, 20.f, 30.f))))
			return E_FAIL;
	}

	/* 콜라이더 이벤트 */
	{
		if (FAILED(Add_Event(L"Tail_Col_On", CEnemy_Event_Collider::Create(this, pTailCollider, true))) ||
			FAILED(Add_Event(L"Tail_Col_Off", CEnemy_Event_Collider::Create(this, pTailCollider, false))) ||
			FAILED(Add_Event(L"Grab_Col_On", CEnemy_Event_Collider::Create(this, pGrabCollider, true))) ||
			FAILED(Add_Event(L"Grab_Col_Off", CEnemy_Event_Collider::Create(this, pGrabCollider, false))) ||
			FAILED(Add_Event(L"Body_Col_On", CEnemy_Event_Collider::Create(this, m_pColliderCom, true))) ||
			FAILED(Add_Event(L"Body_Col_Off", CEnemy_Event_Collider::Create(this, m_pColliderCom, false))) ||
			FAILED(Add_Event(L"Punch_Col_On", CEnemy_Event_Collider::Create(this, pFistCollider, true))) ||
			FAILED(Add_Event(L"Punch_Col_Off", CEnemy_Event_Collider::Create(this, pFistCollider, false))))
			return E_FAIL;
	}

	/* 사운드 이벤트 */
	{
		if (FAILED(Add_Event(L"Sound_Grab", CEnemy_Event_Sound::Create(this, L"Cuttle_Grab_VO1.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Throw", CEnemy_Event_Sound::Create(this, L"cuttle_cast1.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK2))) ||
			FAILED(Add_Event(L"Sound_ExecutionDamage", CEnemy_Event_Sound::Create(this, L"FirthOutro_Crack.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Punch", CEnemy_Event_Sound::Create(this, L"Cuttle_Punch.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))))
			return E_FAIL;
	}

	/* 이펙트 이벤트 */
	{
		if (FAILED(Add_Event(L"Effect_CuttleFish_Ink", CCuttlefish_Event_Effect::Create(this, (_uint)CCuttlefish_Event_Effect::CUTTLE_EFFECT::INK))) ||
			FAILED(Add_Event(L"Effect_CuttleFish_ProjLeft", CCuttlefish_Event_Effect::Create(this, (_uint)CCuttlefish_Event_Effect::CUTTLE_EFFECT::LEFT_CHARGE))) ||
			FAILED(Add_Event(L"Effect_CuttleFish_ProjRight", CCuttlefish_Event_Effect::Create(this, (_uint)CCuttlefish_Event_Effect::CUTTLE_EFFECT::RIGHT_CHARGE))) ||
			FAILED(Add_Event(L"Effect_CuttleFish_Punch_Ready", CCuttlefish_Event_Effect::Create(this, (_uint)CCuttlefish_Event_Effect::CUTTLE_EFFECT::PUNCH_READY))) ||
			FAILED(Add_Event(L"Effect_CuttleFish_Punch", CCuttlefish_Event_Effect::Create(this, (_uint)CCuttlefish_Event_Effect::CUTTLE_EFFECT::PUNCH))) ||
			FAILED(Add_Event(L"Effect_CuttleFish_Punch_Impact", CCuttlefish_Event_Effect::Create(this, (_uint)CCuttlefish_Event_Effect::CUTTLE_EFFECT::PUNCH_IMPACT))) ||
			FAILED(Add_Event(L"Effect_CuttleFish_Grab", CCuttlefish_Event_Effect::Create(this, (_uint)CCuttlefish_Event_Effect::CUTTLE_EFFECT::GRAB))))
			return E_FAIL;
	}

	return S_OK;
}

void CEnemy_Cuttlefish::Throw_Missile_L()
{	
	_vector PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);

	_float4x4 LeftStartMat = m_pModelCom->Get_BoneCombinedMatrix_By_Name("tentacle3_03_L.47_end");	

	m_Missiles[0]->Set_SpawnOption(&LeftStartMat, PlayerPos, 1.8f, 1.2f, true);	
}

void CEnemy_Cuttlefish::Throw_Missile_R()
{
	_vector PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	
	_float4x4 RightStartMat = m_pModelCom->Get_BoneCombinedMatrix_By_Name("tentacle3_03_R.61_end");
	
	m_Missiles[1]->Set_SpawnOption(&RightStartMat, PlayerPos, 1.8f, 1.2f, false);
}

CEnemy_Cuttlefish* CEnemy_Cuttlefish::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Cuttlefish* pInstance = new CEnemy_Cuttlefish(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CEnemy_Cuttlefish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CEnemy_Cuttlefish::Clone(void* pArg)
{
	CEnemy_Cuttlefish* pInstance = new CEnemy_Cuttlefish(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEnemy_Cuttlefish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Cuttlefish::Free()
{
	super::Free();

	for (auto& iter : m_Missiles)
		Safe_Release(iter);
	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pFSMCom);	
	Safe_Release(m_pMovementCom);
	Safe_Release(m_pPhysicsControllerCom);
	Safe_Release(m_pNoiseTextureCom);
}