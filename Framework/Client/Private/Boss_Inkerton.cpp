#include "pch.h"
#include "Boss_Inkerton.h"
#include "GameInstance.h"

//Engine
#include "Movement.h"

//Map Trigger
#include "MapTrigger.h"

//States
#include "Inkerton_StateMachine.h"
#include "Inkerton_EncounterStateMachine.h"

// Events
#include "Enemy_Event_Collider.h"
#include "Enemy_Event_Sound.h"
#include "Inkerton_Event_Move.h"
#include "Inkerton_Event_MoveSide.h"
#include "Inkerton_Event_MoveSpline.h"
#include "Inkerton_Event_Camera.h"
#include "Enemy_Event_AnimSpeed.h"
#include "Inkerton_Event_Fire.h"
#include "Inkerton_Event_InkSpurt.h"
#include "Inkerton_Event_GrabOff.h"
#include "Inkerton_Event_ExecutionDamage.h"

// Effect Events
#include "Inkerton_Event_CineCameraOn.h"
#include "Inkerton_Event_Effect_ChargeAttack.h"
#include "Inkerton_Event_Effect_MixupHeavy.h"
#include "Inkerton_Event_Effect_AimFire.h"
#include "Inkerton_Event_Effect_Slash.h"
#include "Inkerton_Event_Effect_NutCracker.h"
#include "Inkerton_Event_Effect_Execution.h"

//PartObject
#include "UI_Boss.h"
#include "UI_Locked.h"
#include "CineCamera.h"
#include "Inkerton_Gun.h"
#include "Inkerton_Nutcracker.h"
#include "Inkerton_StretchedNutcracker.h"

#include "UI_Manager.h"

CBoss_Inkerton::CBoss_Inkerton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBoss_Inkerton::CBoss_Inkerton(const CBoss_Inkerton& Prototype)
	: super(Prototype)
{
}

HRESULT CBoss_Inkerton::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CBoss_Inkerton::Initialize(void* pArg)
{
	CBoss_Inkerton::Inkerton_DESC* pDesc = static_cast<Inkerton_DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;
	else
	{
		m_bIsEncounter = pDesc->bIsEncounter;		
	}
		

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

	if (FAILED(Ready_BanterSound()))
		return E_FAIL;

	if(m_bIsEncounter)
		m_pFSMCom->Call_NextState(L"Ambush");
	else
		m_pFSMCom->Call_NextState(L"Wait");

	//m_pNPCManager = static_cast<CNPC_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_NPCManager", 0));
	//Safe_AddRef(m_pNPCManager);

	//m_iCurState = m_pNPCManager->Load_State(L"Urchin");

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));	
	return S_OK;
}	

void CBoss_Inkerton::Priority_Update(_float fTimeDelta)
{	
	m_pFSMCom->Priority_Update(this, fTimeDelta);	
	m_pMovementCom->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);	
}

void CBoss_Inkerton::Update(_float fTimeDelta)
{	
	m_pModelCom->Play_Animation(fTimeDelta);	

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsAggroed && !m_bIsEncounter) {
		m_bIsAggroed = Detect_Target(230.f, 45.f, 40.f, m_pPlayerTransform, m_pTransformCom);
		if (m_bIsAggroed)
			m_pFSMCom->Call_NextState(L"Aggroed");
	}

	// Movement
	{
		_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

		m_pFSMCom->Update(this, fTimeDelta);
		Chase(fTimeDelta);
		MoveSide(fTimeDelta);			
		MoveSpline(fTimeDelta);
		m_pMovementCom->Update(fTimeDelta);		

		_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vec3 vDisp = vCurPosition - vPrePosition;

		if (m_pPhysicsControllerCom != nullptr)
			m_pPhysicsControllerCom->Move(vDisp, static_cast<_float>(fTimeDelta * 0.5), fTimeDelta);	
	}

	__super::Update(fTimeDelta);
}

void CBoss_Inkerton::Late_Update(_float fTimeDelta)
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

	__super::Late_Update(fTimeDelta);
}


HRESULT CBoss_Inkerton::Render()
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

HRESULT CBoss_Inkerton::Render_Shadow()
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

void CBoss_Inkerton::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
}

void CBoss_Inkerton::On_Collision(CGameObject* pOther, _uint iGroup)
{

}

void CBoss_Inkerton::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{

}

void CBoss_Inkerton::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
{
	if (m_bIsDead)
		return;

	m_bIsDamaged = true;	
	Reset_RecoverDelay();
	m_EnemyStats.fCurHP -= fDamage;
	
	if (m_bIsEncounter && (m_EnemyStats.fCurHP < m_EnemyStats.fMaxHP - (m_EnemyStats.fMaxHP / 4))) {
		m_bIsDead = true;
		m_pFSMCom->Call_NextState(L"Retreat");
		return;
	}
	else if (m_EnemyStats.fCurHP <= 0) {
		m_EnemyStats.fCurHP = 0;
		m_bIsDead = true;
		m_pFSMCom->Call_NextState(L"Death");
		return;
	}

	if (m_EnemyStats.bIsBalanceBreakable && eDamage == DAMAGE::DAMAGE_HEAVY) {
		m_pFSMCom->Call_NextState(L"Capsized");
		Reset_Balance();
		m_EnemyStats.bIsCapsized = true;
		m_EnemyStats.bIsBalanceBreakable = false;
		return;
	}

	if (!m_EnemyStats.bIsBalanceBreakable && !m_EnemyStats.bIsCapsized) {
		m_EnemyStats.fCurBP += fDamage;

		if (m_EnemyStats.fCurBP >= m_EnemyStats.fMaxBP) {
			m_EnemyStats.fCurBP = m_EnemyStats.fMaxBP;
			m_EnemyStats.bIsBalanceBreakable = true;
		}
	}	

	CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Sword4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.9f);
}

void CBoss_Inkerton::Parried()
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

void CBoss_Inkerton::Active()
{
	if (m_bIsDead || m_bIsEncounter)
		return;

	Ready_Physics();

	m_pMovementCom->Set_GravityScale(10.f);
	m_pMovementCom->Set_MaxFallingSpeed(10.f);

	super::Active();
}

void CBoss_Inkerton::Deactive()
{
	if (m_bIsDead && !m_bIsEncounter)
	{
		CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Death", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);
		CGameInstance::GetInstance()->FadeSwap_Sound(L"Logo", (_uint)SOUND_CHANNEL::BGM, 0.1f, 0.5f);
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Show_CreditUI();
	}
	
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);
	Delete_Physics();	

	super::Deactive();
}

HRESULT CBoss_Inkerton::Ready_Components()
{
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
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Boss_Inkerton",
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
		if (m_bIsEncounter) {
			CInkerton_EncounterStateMachine::IMKERTON_FSM_DESC Desc{};
			Desc.pFSMDesc.eMode = CFSM::FSM_RAND;
			Desc.pFSMDesc.pOwner = this;

			if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Inkerton_EncounterStateMachine",
				L"Com_StateMachine", reinterpret_cast<CComponent**>(&m_pFSMCom), &Desc)))
				return E_FAIL;

			m_isActive = false;
		}
		else {
			CInkerton_StateMachine::IMKERTON_FSM_DESC Desc{};
			Desc.pFSMDesc.eMode = CFSM::FSM_RAND;
			Desc.pFSMDesc.pOwner = this;

			if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Inkerton_StateMachine",
				L"Com_StateMachine", reinterpret_cast<CComponent**>(&m_pFSMCom), &Desc)))
				return E_FAIL;
		}

		m_pFSMCom->Refresh_AllStates();
	}

	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.fRadius = 1.5f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Inkerton::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fRadius = 1.f;
		Desc.fHeight = 1.f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Inkerton::Ready_Events()
{
	CCollider* pJointCollider = static_cast<CInkerton_Nutcracker*>(Find_PartObject({ L"Part_Nutcracker" }))->Get_Collider();
	CCollider* pHandleCollider = static_cast<CInkerton_Nutcracker*>(Find_PartObject({ L"Part_Nutcracker" }))->Get_HandleCollider();
	CCollider* pStretchCollider = static_cast<CInkerton_StretchedNutcracker*>(Find_PartObject({ L"Part_StretchedNutcracker" }))->Get_Collider();

	//이동 이벤트
	if (FAILED(Add_Event(L"Move_Forward", CInkerton_Event_Move::Create(this, true, 1.5f))) ||	
		FAILED(Add_Event(L"Move_Back", CInkerton_Event_Move::Create(this, true, -1.5f))) ||
		FAILED(Add_Event(L"Move_Slow", CInkerton_Event_Move::Create(this, true, 0.5f))) ||
		FAILED(Add_Event(L"Swim_Forward", CInkerton_Event_Move::Create(this, true, 3.5f))) ||
		FAILED(Add_Event(L"Swim_Back", CInkerton_Event_Move::Create(this, true, -3.5f))) ||
		FAILED(Add_Event(L"Move_Look", CInkerton_Event_Move::Create(this, true, 0.f))) ||
		FAILED(Add_Event(L"Move_Left", CInkerton_Event_MoveSide::Create(this, true, -1.5f))) ||
		FAILED(Add_Event(L"Move_Right", CInkerton_Event_MoveSide::Create(this, true, 1.5f))) ||
		FAILED(Add_Event(L"Move_Stop", CInkerton_Event_Move::Create(this, false))) ||
		FAILED(Add_Event(L"SideMove_Stop", CInkerton_Event_MoveSide::Create(this, false))) ||
		FAILED(Add_Event(L"Move_Spline_Heavy", CInkerton_Event_MoveSpline::Create(this, true, 85.f))) ||
		FAILED(Add_Event(L"Move_Spline_Light", CInkerton_Event_MoveSpline::Create(this, true, 66.5f))) ||
		FAILED(Add_Event(L"LDodge_Move_Spline", CInkerton_Event_MoveSpline::Create(this, true, 20.f))) ||
		FAILED(Add_Event(L"Stop_Spline", CInkerton_Event_MoveSpline::Create(this, false))))
		return E_FAIL;
	
	/* 애니메이션 속도 이벤트 */
	if (FAILED(Add_Event(L"Anim_Slow", CEnemy_Event_AnimSpeed::Create(this, 0.5f))) ||
		FAILED(Add_Event(L"Charge_Anim_Fast", CEnemy_Event_AnimSpeed::Create(this, 1.3f))) ||		
		FAILED(Add_Event(L"LDodge_Anim_Slow", CEnemy_Event_AnimSpeed::Create(this, 0.7f))) ||
		FAILED(Add_Event(L"Light_Anim_Slow", CEnemy_Event_AnimSpeed::Create(this, 0.9f))) ||
		FAILED(Add_Event(L"Anim_Fast_Combo3", CEnemy_Event_AnimSpeed::Create(this, 1.3f))) ||
		FAILED(Add_Event(L"Anim_Slow_Aim", CEnemy_Event_AnimSpeed::Create(this, 0.6f))) ||
		FAILED(Add_Event(L"Anim_Nor", CEnemy_Event_AnimSpeed::Create(this, 1.0f))))
		return E_FAIL;

	/* 콜라이더 이벤트 */
	if (FAILED(Add_Event(L"Joint_Col_On", CEnemy_Event_Collider::Create(this, pJointCollider, true))) ||
		FAILED(Add_Event(L"Joint_Col_Off", CEnemy_Event_Collider::Create(this, pJointCollider, false))) ||
		FAILED(Add_Event(L"Handle_Col_On", CEnemy_Event_Collider::Create(this, pHandleCollider, true))) ||
		FAILED(Add_Event(L"Handle_Col_Off", CEnemy_Event_Collider::Create(this, pHandleCollider, false))) ||
		FAILED(Add_Event(L"Stretch_Col_On", CEnemy_Event_Collider::Create(this, pStretchCollider, true))) ||
		FAILED(Add_Event(L"Stretch_Col_Off", CEnemy_Event_Collider::Create(this, pStretchCollider, false))))
		return E_FAIL;

	/* 공격 이벤트 */
	if (FAILED(Add_Event(L"Fire_Bullet", CInkerton_Event_Fire::Create(this))) ||
		FAILED(Add_Event(L"Ink_Spurt", CInkerton_Event_InkSpurt::Create(this)))||
		FAILED(Add_Event(L"Execution_Crack_Damage", CInkerton_Event_ExecutionDamage::Create(this, 30.f, 10.f))) ||
		FAILED(Add_Event(L"Execution_Slam_Damage", CInkerton_Event_ExecutionDamage::Create(this, 10.f, 2.f))) ||
		FAILED(Add_Event(L"Grab_Off", CInkerton_Event_GrabOff::Create(this))))
		return E_FAIL;
	
	//카메라 이벤트
	//if (FAILED(Add_Event(L"CineCamera_On", CInkerton_Event_CineCameraOn::Create(this))))
	//	return E_FAIL;

	/* 사운드 이벤트 */
	{
		if (FAILED(Add_Event(L"Sound_HeavyCharge", CEnemy_Event_Sound::Create(this, L"Inkerton_Attack_Open_4.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_HaveyAttack", CEnemy_Event_Sound::Create(this, L"Inkerton_Attack_4.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_HeavySlam", CEnemy_Event_Sound::Create(this, L"Inkerton_Attack_Open_3.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_LightCharge", CEnemy_Event_Sound::Create(this, L"Inkerton_Attack_Open_2.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_LightAttack", CEnemy_Event_Sound::Create(this, L"Inkerton_Attack_Open_1.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_JointSnap", CEnemy_Event_Sound::Create(this, L"Inkerton_Attack_Open_2.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK2))) ||
			FAILED(Add_Event(L"Sound_Crack", CEnemy_Event_Sound::Create(this, L"Nutcracker_Crush_Exciter_1.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK2, L"이 부서지는 소리... 정말 질리지 않아.", 4.f))) ||
			FAILED(Add_Event(L"Sound_ExecutionSlam", CEnemy_Event_Sound::Create(this, L"Nutcracker_Crush_Slam.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK3))) ||
			FAILED(Add_Event(L"Sound_Aim", CEnemy_Event_Sound::Create(this, L"Inkerton_Aim.wav", 0.8f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Reload", CEnemy_Event_Sound::Create(this, L"Inkerton_Reload.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))))
			return E_FAIL;
	}

	const _float4x4* pNutCrackerMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("HB_MixupHeavy.26");
	const _float4x4* pGunMuzzleMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Gun_end.6");
	/* 이펙트 이벤트 */
	if (FAILED(Add_Event(L"Charge_Effect", CInkerton_Event_Effect_ChargeAttack::Create(this, pNutCrackerMatrix))) ||
		FAILED(Add_Event(L"Heavy_Effect", CInkerton_Event_Effect_MixupHeavy::Create(this, pNutCrackerMatrix))) ||
		FAILED(Add_Event(L"Aim_Effect", CInkerton_Event_Effect_AimFire::Create(this, pGunMuzzleMatrix, false))) ||
		FAILED(Add_Event(L"Fire_Effect", CInkerton_Event_Effect_AimFire::Create(this, pGunMuzzleMatrix, true))) ||
		FAILED(Add_Event(L"Slash_Effect_Combo1", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 1))) ||
		FAILED(Add_Event(L"Slash_Effect_Combo2", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 2))) ||
		FAILED(Add_Event(L"Slash_Effect_Combo2_2", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 4))) ||
		FAILED(Add_Event(L"Slash_Effect_Combo3", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 3))) ||
		FAILED(Add_Event(L"Slash_Effect_LeftDodge4", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 5))) ||
		FAILED(Add_Event(L"Slash_Effect_MixupLight5", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 6))) ||
		FAILED(Add_Event(L"NutCracker_Effect_Ready", CInkerton_Event_Effect_NutCracker::Create(this, pNutCrackerMatrix, true))) ||
		FAILED(Add_Event(L"NutCracker_Effect_Impact", CInkerton_Event_Effect_NutCracker::Create(this, pNutCrackerMatrix, false))) ||
		FAILED(Add_Event(L"Execution_Effect", CInkerton_Event_Effect_Execution::Create(this, pNutCrackerMatrix, false))) || 
		FAILED(Add_Event(L"Execution_Slam_Effect", CInkerton_Event_Effect_Execution::Create(this, pNutCrackerMatrix, true))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Inkerton::Ready_BanterSound()
{
	m_pBanterSound.push_back(L"Inkerton_Banter_1.wav");
	m_pBanterSound.push_back(L"Inkerton_Banter_2.wav");
	m_pBanterSound.push_back(L"Inkerton_Banter_3.wav");
	m_pBanterSound.push_back(L"Inkerton_Banter_4.wav");
	m_pBanterSound.push_back(L"Inkerton_Banter_5.wav");
	m_pBanterSound.push_back(L"Inkerton_Banter_7.wav");

	m_pBanterSoundDuration.push_back(2.5f);	
	m_pBanterSoundDuration.push_back(2.5f);
	m_pBanterSoundDuration.push_back(2.5f);
	m_pBanterSoundDuration.push_back(2.5f);
	m_pBanterSoundDuration.push_back(2.5f);
	m_pBanterSoundDuration.push_back(2.f);

	m_pBanterSoundQuote.push_back(L"같이 지옥을 겪자, 꼬마야");
	m_pBanterSoundQuote.push_back(L"날 쉽게 이기려 들지 마");
	m_pBanterSoundQuote.push_back(L"지옥에나 떨어져라");
	m_pBanterSoundQuote.push_back(L"재수 없는 꼬맹이 스펀지 같으니");
	m_pBanterSoundQuote.push_back(L"빌어먹을 조그만 따개비 녀석");
	m_pBanterSoundQuote.push_back(L"지저분한 작은 이끼벌레 같으니");

	return S_OK;
}

HRESULT CBoss_Inkerton::Ready_PartObject()
{
	// Nutcracker_L.15
	// Nutcracker_L.15_end
	// Nutcracker_L.15_end_end
	// Nutcracker_R.36
	// Nutcracker_R.36_end
	// Nutcracker_R.36_end_end
	
	 /* Nutcracker */
	{
		CInkerton_Nutcracker::DESC NutcrackerDesc{};

		NutcrackerDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();		
		NutcrackerDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Nutcracker_R.36");
		NutcrackerDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_Nutcracker", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Inkerton_Nutcracker", &NutcrackerDesc)))
			return E_FAIL;	
	}

	/* Stretched Nutcracker */
	{
		CInkerton_StretchedNutcracker::DESC StretchedNutcracker{};

		StretchedNutcracker.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		StretchedNutcracker.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Nutcracker_L.15");
		StretchedNutcracker.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_StretchedNutcracker", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Inkerton_StretchedNutcracker", &StretchedNutcracker)))
			return E_FAIL;		
	}

	/* Stretched Prototype_GameObject_Inkerton_Gun */
	{
		CInkerton_Gun::DESC StretchedNutcracker{};

		StretchedNutcracker.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		StretchedNutcracker.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Gun_end.6");
		StretchedNutcracker.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_Gun", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Inkerton_Gun", &StretchedNutcracker)))
			return E_FAIL;
	}

	// UI
	{
		CUI_Boss::DESC BossDesc{};
		{
			BossDesc.pEnemyStats = &m_EnemyStats;
			BossDesc.pIsAggroed = &m_bIsAggroed;
			BossDesc.pIsDead = &m_bIsDead;
			BossDesc.pIsGrab = &m_bIsGrab;

			BossDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
			BossDesc.pCenterMatrixPtr = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Hip_x.55");

			lstrcpy(BossDesc.szName, m_szName);
		}
		if (FAILED(Add_PartObject(L"Part_UI_Boss", LEVEL_STATIC, L"Prototype_GameObject_UI_Boss", &BossDesc)))
			return E_FAIL;
	}

	// CineCamera
	{
		CCineCamera::DESC CineCameraDesc{};

		CineCameraDesc.CameraDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
		CineCameraDesc.CameraDesc.fNear = 0.1f;
		CineCameraDesc.CameraDesc.fFar = 1000.f;
		CineCameraDesc.pOwner = this;
		CineCameraDesc.strName = L"CineCamera_Inkerton";

		if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
			return E_FAIL;

		m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
		m_pCineCamera->Add_Cinematic(L"Inkerton_Execute", "../Bin/DataFiles/CameraData/Inkerton_Execute.data");
	}

	return S_OK;
}

void CBoss_Inkerton::Ambush()
{
	if (m_bIsDead || m_isActive)
		return;

	m_isActive = true;
	m_bIsAggroed = true;	
}

void CBoss_Inkerton::Locate(_fvector _vPos)
{
	Active();
	if (nullptr != m_pPhysicsControllerCom)
		m_pPhysicsControllerCom->Set_Position(_vPos);
	m_bIsAggroed = true;

	m_OriginalPos._41 = XMVectorGetX(_vPos);
	m_OriginalPos._42 = XMVectorGetY(_vPos);
	m_OriginalPos._43 = XMVectorGetZ(_vPos);
}

void CBoss_Inkerton::Delete_Physics()
{
	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}
}

void CBoss_Inkerton::Chase(_float fTimeDelta)
{
	if (!m_bIsChase)
		return;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);	

	if (fDis < 2.f && m_fChaseSpeed > 0) {
		m_bIsChase = false;
		return;
	}

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Straight(fTimeDelta * m_fChaseSpeed);
}

void CBoss_Inkerton::MoveSide(_float fTimeDelta)
{
	if (!m_bIsSideMove)
		return;

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Right(fTimeDelta * m_fSideMoveSpeed);
}

void CBoss_Inkerton::MoveSpline(_float fTimeDelta)
{
	if (!m_bIsSplineMove)
		return;

	_vector temp = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vec2 dest = _vec2(XMVectorGetX(temp), XMVectorGetZ(temp));
	_vec2 vDis = m_SplineRoutes->at(m_iCurSplineRouteIndex);

	_float fDisX = dest.x - vDis.x;
	_float fDisZ = dest.y - vDis.y;
	_float fDistance = sqrt(fDisX * fDisX + fDisZ * fDisZ);

	if (fDistance < 1.5f)
		m_iCurSplineRouteIndex++;

	if (m_SplineRoutes->size() <= m_iCurSplineRouteIndex) {
		m_bIsSplineMove = false;
		m_pTransformCom->Set_Speed(1.f);
		m_iCurSplineRouteIndex = 0;
		return;
	}

	_vector vTargetPos = XMVectorSet(m_SplineRoutes->at(m_iCurSplineRouteIndex).x, 0.f, m_SplineRoutes->at(m_iCurSplineRouteIndex).y, 1.f);
	m_pTransformCom->Set_Speed(m_fSplineSpeed);	

	_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::POSITION));
	vTargetPos = XMVectorSetY(vTargetPos, fY);	
	m_pTransformCom->LookAt(vTargetPos);
	m_pTransformCom->Go_Straight(fTimeDelta);
}

_vec2 CBoss_Inkerton::Compute_CardinalSpline(const _vec2& p0, const _vec2& p1, _float fFactor, _float fTension) {
	_vec2 vAB = p1 - p0;
	_vec2 vLeftTangent = _vec2(-vAB.y, vAB.x);

	_float m = fTension;

	_float fTangentEffect = m * fFactor * (1 - fFactor);

	_vec2 vResult;
	vResult.x = p0.x * (1 - fFactor) + p1.x * fFactor + fTangentEffect * vLeftTangent.x;
	vResult.y = p0.y * (1 - fFactor) + p1.y * fFactor + fTangentEffect * vLeftTangent.y;

	return vResult;
}

void CBoss_Inkerton::Select_Action()
{
	if (!m_bIsAggroed) {
		m_pFSMCom->Call_NextState(L"Wait");
		return;
	}

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);	
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 8) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CInkerton_StateMachine::STATE_CLOSE);
	}
	else if (fDis < 25) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CInkerton_StateMachine::STATE_RANGED);
	}
	else if (fDis < 45) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CInkerton_StateMachine::STATE_FAR);
	}
	else {
		//m_pFSMCom->Select_StatesFromGroup((_uint)CInkerton_StateMachine::STATE_OUTOFRANGE);
		m_pFSMCom->Call_NextState(L"Wait");
		m_bIsAggroed = false;
	}
}

void CBoss_Inkerton::Play_BaterQuote()
{
	_int rand = CUtils::Compute_Random(0, (m_pBanterSound.size() - 1) * 2);
	if (rand > m_pBanterSound.size() - 1)
		return;

	CGameInstance::GetInstance()->Play_Sound(m_pBanterSound[rand], (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.5f);

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Show_Caption(m_pBanterSoundQuote[rand].c_str(), m_pBanterSoundDuration[rand]);	
}

void CBoss_Inkerton::Respawn()
{
	if (m_bIsDead)
		return;

	if (FAILED(Ready_Physics()))
		return;

	if (m_bIsEncounter)
		m_isActive = false;

	_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	super::Respawn();
	_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	m_pPhysicsControllerCom->Set_Position(vCurPosition);

	m_bIsChase = false;
	m_fDissolveRate = 2.f;
	m_pColliderCom->Set_Active(true);
	m_pFSMCom->Call_NextState(L"Wait");
}

void CBoss_Inkerton::Death()
{
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);
	Delete_Physics();
	
	m_pColliderCom->Set_Active(false);

	super::Death();
}

CBoss_Inkerton* CBoss_Inkerton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Inkerton* pInstance = new CBoss_Inkerton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CBoss_Inkerton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBoss_Inkerton::Clone(void* pArg)
{
	CBoss_Inkerton* pInstance = new CBoss_Inkerton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CBoss_Inkerton");
		Safe_Release(pInstance);
	}

	return pInstance;
}
 
void CBoss_Inkerton::Free()
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

