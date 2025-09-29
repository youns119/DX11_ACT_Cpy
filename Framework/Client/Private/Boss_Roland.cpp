#include "pch.h"
#include "Boss_Roland.h"
#include "GameInstance.h"
#include "Player.h"

#include "Interactive_Bounceable.h"

//States
#include "Roland_StateMachine.h"

// Events
#include "Enemy_Event_Collider.h"
#include "Enemy_Event_Sound.h"
#include "Roland_Event_WeaponEquipment.h"
#include "Roland_Event_Warning.h"
#include "Roland_Event_Move.h"
#include "Roland_Event_MoveSide.h"

// Effect Events
#include "EffectObject.h"
#include "Roland_Event_PunchEffect.h"
#include "Roland_Event_ThrustEffect.h"
#include "Roland_Event_MultiThrustEffect.h"
#include "Roland_Event_SlashEffect.h"
#include "Roland_Event_GolfHitEffect.h"
#include "Roland_Event_GolfEffect.h"
#include "Roland_Event_WallopEffect.h"
#include "Roland_Event_BigComboEffect.h"
#include "Roland_Event_BigSwipeEffect.h"
#include "Roland_Event_BellyFlopEffect.h"

//PartObject
#include "UI_Boss.h"
#include "UI_Locked.h"
#include "Roland_Fist.h"
#include "Roland_Belly.h"
#include "Roland_Wrench_0_L.h"
#include "Roland_Wrench_1_L.h"
#include "Roland_Wrench_2_L.h"
#include "Roland_Wrench_0_R.h"
#include "Roland_Wrench_1_R.h"
#include "Roland_Wrench_2_R.h"


CBoss_Roland::CBoss_Roland(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBoss_Roland::CBoss_Roland(const CBoss_Roland& Prototype)
	: super(Prototype)
{
}

HRESULT CBoss_Roland::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	//	m_pPhysicsControllerCom->Set_UserData({true});
	return S_OK;
}

HRESULT CBoss_Roland::Initialize(void* pArg)
{
	CBoss_Roland::DESC* pDesc = static_cast<DESC*>(pArg);

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

	if (FAILED(Ready_Bounceable()))
		return E_FAIL;

	{/* PointLight */

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		vPos = XMVectorSetY(vPos, (XMVectorGetY(vPos) + 2.f));

		LIGHT_DESC LightDesc = {};
		LightDesc.vDirection = _float4(0.f, 1.f, 0.05f, 0.f);
		LightDesc.vCosCutOff = _float2(50.f, 80.f);


		LightDesc.eType = LIGHT_TYPE::SPOT;
		LightDesc.fRange = 80.f;
		XMStoreFloat4(&LightDesc.vPosition, vPos);
		LightDesc.vDiffuse = _float4(4.5f, 3.4f, 0.6f, 1.f);
		LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
		//LightDesc.vSpecular = m_vSpecular;
		LightDesc.vAttenuation = _float3(1.f, 0.2461f, 0.9f);

		_wstring strLightTag = L"Light_Roland";
		m_pGameInstance->Add_Light(strLightTag, LightDesc);

	}

	m_pFSMCom->Call_NextState(L"Wait");

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
		
	return S_OK;
}

void CBoss_Roland::Priority_Update(_float fTimeDelta)
{
	m_pFSMCom->Priority_Update(this, fTimeDelta);
	m_pMovementCom->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CBoss_Roland::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsAggroed) {
		m_bIsAggroed = Detect_Target(230.f, 35.f, 35.f, m_pPlayerTransform, m_pTransformCom);
		if (m_bIsAggroed)
			m_pFSMCom->Call_NextState(L"Aggroed");
	}

	// Movement
	{
		_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

		m_pFSMCom->Update(this, fTimeDelta);
		Chase(fTimeDelta);
		MoveSide(fTimeDelta);
		Setup_Light();
		m_pMovementCom->Update(fTimeDelta);

		_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vec3 vDisp = vCurPosition - vPrePosition;

		if (m_pPhysicsControllerCom != nullptr)
			m_pPhysicsControllerCom->Move(vDisp, static_cast<_float>(fTimeDelta * 0.5), fTimeDelta);
	}	

	__super::Update(fTimeDelta);
}

void CBoss_Roland::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

	if (nullptr != m_pPhysicsControllerCom) {
		m_pGameInstance->Simulate_Physics(m_pPhysicsControllerCom);

		if (m_bIsRoll)
			m_pPhysicsControllerCom->Set_UserData({ true });
		else
			m_pPhysicsControllerCom->Set_UserData({ false });

#ifdef _DEBUG
		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsControllerCom);
#endif // _DEBUG

		if (m_bIsRoll)
			m_pPhysicsControllerCom->Set_UserData({ true });
		else
			m_pPhysicsControllerCom->Set_UserData({ false });
	}

	__super::Late_Update(fTimeDelta);
}


HRESULT CBoss_Roland::Render()
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

	if (m_bIsDamaged) {
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDamagedIntensity", &m_fFlashIntensity, sizeof(_float))))
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
		else if(m_bIsDamaged)
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

HRESULT CBoss_Roland::Render_Shadow()
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

void CBoss_Roland::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr && m_bIsRoll) {
		pPlayer->Damaged(10.f, IDamaged::DAMAGE::DAMAGE_NORMAL, this);		
		CGameInstance::GetInstance()->Play_Sound(L"Roland_RollPlayerImpact", (_uint)SOUND_CHANNEL::ENEMY_ATTACK3, 1.f);
	}		

	if (iGroup == (_uint)COLLISION_GROUP::PINBALL) {
		if (m_bIsRoll)
			m_bIsBounced = true;
	}
}

void CBoss_Roland::On_Collision(CGameObject* pOther, _uint iGroup)
{
	
}

void CBoss_Roland::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
	if (iGroup == (_uint)COLLISION_GROUP::PINBALL) {
		if (m_bIsRoll)
			m_bIsBounced = false;
	}
}

void CBoss_Roland::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
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
		m_EnemyStats.bIsBalanceBreakable = false;
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Sword4.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.9f);
}

void CBoss_Roland::Parried()
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

void CBoss_Roland::Active()
{
	if (m_bIsDead)
		return;

	Ready_Physics();

	m_pMovementCom->Set_GravityScale(10.f);
	m_pMovementCom->Set_MaxFallingSpeed(10.f);

	super::Active();
}

void CBoss_Roland::Deactive()
{
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);
	Delete_Physics();

	super::Deactive();
}

HRESULT CBoss_Roland::Ready_Components()
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
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Boss_Roland",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;

	if (m_pMovementCom == nullptr)
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 50.f;
		Desc.fMoveDecel = 0.f;
		Desc.fMaxSpeed = 5.f;
		Desc.fMaxFallingSpeed = 10.f;
		Desc.fRotationScale = 1.f;
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

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Roland_StateMachine",
			L"Com_StateMachine", reinterpret_cast<CComponent**>(&m_pFSMCom), &Desc)))
			return E_FAIL;

		m_pFSMCom->Refresh_AllStates();
	}

	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.fRadius = 2.0f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CBoss_Roland::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fScaleCoeff = 0.5f;
		Desc.fRadius = 1.0f;
		Desc.fHeight = 0.35f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Roland::Ready_Events()
{
	CCollider* FistCollider = static_cast<CRoland_Fist*>(Find_PartObject({ L"Part_Fist" }))->Get_Collider();
	CCollider* BellyCollider = static_cast<CRoland_Belly*>(Find_PartObject({ L"Part_Belly" }))->Get_Collider();
	CCollider* pWrench_0_R_Collider = static_cast<CRoland_Wrench_0_R*>(Find_PartObject({ L"Part_Wrench_0_R" }))->Get_Collider();
	CCollider* pWrench_0_RB_Collider = static_cast<CRoland_Wrench_0_R*>(Find_PartObject({ L"Part_Wrench_0_R" }))->Get_BottomCollider();
	CCollider* pWrench_1_R_Collider = static_cast<CRoland_Wrench_1_R*>(Find_PartObject({ L"Part_Wrench_1_R" }))->Get_Collider();
	CCollider* pWrench_2_R_Collider = static_cast<CRoland_Wrench_2_R*>(Find_PartObject({ L"Part_Wrench_2_R" }))->Get_Collider();
	CCollider* pWrench_0_L_Collider = static_cast<CRoland_Wrench_0_L*>(Find_PartObject({ L"Part_Wrench_0_L" }))->Get_Collider();
	CCollider* pWrench_1_L_Collider = static_cast<CRoland_Wrench_1_L*>(Find_PartObject({ L"Part_Wrench_1_L" }))->Get_Collider();
	CCollider* pWrench_2_L_Collider = static_cast<CRoland_Wrench_2_L*>(Find_PartObject({ L"Part_Wrench_2_L" }))->Get_Collider();

	//이동 이벤트
	if (FAILED(Add_Event(L"Move_Forward", CRoland_Event_Move::Create(this, true, 1.f))) ||
		FAILED(Add_Event(L"Thrust_Move_Forward", CRoland_Event_Move::Create(this, true, 1.5f))) ||
		FAILED(Add_Event(L"GolfHop_Move_Forward", CRoland_Event_Move::Create(this, true, 0.5f))) ||
		FAILED(Add_Event(L"Move_Look", CRoland_Event_Move::Create(this, true, 0.f))) ||
		FAILED(Add_Event(L"Move_Left", CRoland_Event_MoveSide::Create(this, true, -1.1f))) ||
		FAILED(Add_Event(L"Move_Right", CRoland_Event_MoveSide::Create(this, true, 1.1f))) ||
		FAILED(Add_Event(L"Move_Stop", CRoland_Event_Move::Create(this, false))) ||
		FAILED(Add_Event(L"Side_Move_Stop", CRoland_Event_MoveSide::Create(this, false))))
		return E_FAIL;

	/* 무기 꺼내기 & 넣기 이벤트 */
	if (FAILED(Add_Event(L"Reveal_Weapon", CRoland_Event_WeaponEquipment::Create(this, true))) ||
		FAILED(Add_Event(L"Hide_Weapon", CRoland_Event_WeaponEquipment::Create(this, false))))
		return E_FAIL;

	/* 경고 이벤트 */
	if (FAILED(Add_Event(L"Unblockable_On", CRoland_Event_Warning::Create(this, true))) ||
		FAILED(Add_Event(L"Unblockable_Off", CRoland_Event_Warning::Create(this, false))))
		return E_FAIL;

	/* 콜라이더 이벤트 */
	if (FAILED(Add_Event(L"0_R_Col_On", CEnemy_Event_Collider::Create(this, pWrench_0_R_Collider, true))) ||		
		FAILED(Add_Event(L"0_RB_Col_On", CEnemy_Event_Collider::Create(this, pWrench_0_RB_Collider, true))) ||
		FAILED(Add_Event(L"1_R_Col_On", CEnemy_Event_Collider::Create(this, pWrench_1_R_Collider, true))) ||				
		FAILED(Add_Event(L"2_R_Col_On", CEnemy_Event_Collider::Create(this, pWrench_2_R_Collider, true))) ||
		FAILED(Add_Event(L"0_R_Col_Off", CEnemy_Event_Collider::Create(this, pWrench_0_R_Collider, false))) ||
		FAILED(Add_Event(L"0_RB_Col_Off", CEnemy_Event_Collider::Create(this, pWrench_0_RB_Collider, false))) ||
		FAILED(Add_Event(L"1_R_Col_Off", CEnemy_Event_Collider::Create(this, pWrench_1_R_Collider, false))) ||
		FAILED(Add_Event(L"2_R_Col_Off", CEnemy_Event_Collider::Create(this, pWrench_2_R_Collider, false))) ||

		FAILED(Add_Event(L"0_L_Col_On", CEnemy_Event_Collider::Create(this, pWrench_0_L_Collider, true))) ||		
		FAILED(Add_Event(L"1_L_Col_On", CEnemy_Event_Collider::Create(this, pWrench_1_L_Collider, true))) ||
		FAILED(Add_Event(L"2_L_Col_On", CEnemy_Event_Collider::Create(this, pWrench_2_L_Collider, true))) ||
		FAILED(Add_Event(L"0_L_Col_Off", CEnemy_Event_Collider::Create(this, pWrench_0_L_Collider, false))) ||
		FAILED(Add_Event(L"1_L_Col_Off", CEnemy_Event_Collider::Create(this, pWrench_1_L_Collider, false))) ||
		FAILED(Add_Event(L"2_L_Col_Off", CEnemy_Event_Collider::Create(this, pWrench_2_L_Collider, false))) ||
		
		FAILED(Add_Event(L"Flop_Col_On", CEnemy_Event_Collider::Create(this, BellyCollider, true))) ||
		FAILED(Add_Event(L"Flop_Col_Off", CEnemy_Event_Collider::Create(this, BellyCollider, false))) ||

		FAILED(Add_Event(L"Offhand_Col_On", CEnemy_Event_Collider::Create(this, FistCollider, true))) ||
		FAILED(Add_Event(L"Offhand_Col_Off", CEnemy_Event_Collider::Create(this, FistCollider, false))))
		return E_FAIL;
	
	/* 사운드 이벤트 */
	{
		if (FAILED(Add_Event(L"Sound_LeftSwing", CEnemy_Event_Sound::Create(this, L"Roland ShortSwing 1.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) || 
			FAILED(Add_Event(L"Sound_RightSwing", CEnemy_Event_Sound::Create(this, L"Roland ShortSwing 2.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_ImpactDefaultNew", CEnemy_Event_Sound::Create(this, L"Roland ImpactDefault New.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK2))) ||
			FAILED(Add_Event(L"Sound_ImpactDefault", CEnemy_Event_Sound::Create(this, L"Roland ImpactDefault.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_PunchAttempt", CEnemy_Event_Sound::Create(this, L"Roland_PunchAttempt.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_GolfSwing", CEnemy_Event_Sound::Create(this, L"1Roland GolfSwing.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Business", CEnemy_Event_Sound::Create(this, L"Roland-Business.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_VOICE2, L"그냥 일일 뿐이야 꼬맹아.", 4.f))) ||
			FAILED(Add_Event(L"Sound_GolfHHop", CEnemy_Event_Sound::Create(this, L"Roland_Footstep_1.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))))
			return E_FAIL;
	}

	/* 이펙트 이벤트 */
	const _float4x4* pWrench_0_R_Matrix = &Find_PartObject({ L"Part_Wrench_0_R" })->Get_CombinedMatrix();
	const _float4x4* pWrench_0_L_Matrix = &Find_PartObject({ L"Part_Wrench_0_L" })->Get_CombinedMatrix();
	const _float4x4* pWrench_2_R_Matrix = &Find_PartObject({ L"Part_Wrench_2_R" })->Get_CombinedMatrix();
	const _float4x4* pWrench_2_L_Matrix = &Find_PartObject({ L"Part_Wrench_2_L" })->Get_CombinedMatrix();
	if (FAILED(Add_Event(L"OffHandPunch_Effects", CRoland_Event_PunchEffect::Create(this, &Find_PartObject({ L"Part_Fist" })->Get_CombinedMatrix()))) ||
		FAILED(Add_Event(L"SingleThrust_Effects", CRoland_Event_ThrustEffect::Create(this, pWrench_0_R_Matrix))) ||
		FAILED(Add_Event(L"MultiThrust_Effects", CRoland_Event_MultiThrustEffect::Create(this, pWrench_0_R_Matrix))) ||
		FAILED(Add_Event(L"SlashLeft_Effect", CRoland_Event_SlashEffect::Create(this, true))) || 
		FAILED(Add_Event(L"SlashRight_Effect", CRoland_Event_SlashEffect::Create(this, false))) ||
		FAILED(Add_Event(L"Golf_Effects", CRoland_Event_GolfEffect::Create(this))) || 
		FAILED(Add_Event(L"Flop_Effect", CRoland_Event_BellyFlopEffect::Create(this))) ||
		FAILED(Add_Event(L"GolfHit_Effects", CRoland_Event_GolfHitEffect::Create(this, pWrench_0_R_Matrix))) ||
		FAILED(Add_Event(L"Wallop_Effects", CRoland_Event_WallopEffect::Create(this, false, pWrench_0_R_Matrix))) ||
		FAILED(Add_Event(L"Wallop_Effects_Hit", CRoland_Event_WallopEffect::Create(this, true, pWrench_0_R_Matrix))) || 
		FAILED(Add_Event(L"BigCombo_Effects_Combo0", CRoland_Event_BigComboEffect::Create(this, pWrench_2_R_Matrix, 0))) ||
		FAILED(Add_Event(L"BigCombo_Effects_Combo1", CRoland_Event_BigComboEffect::Create(this, pWrench_0_R_Matrix, 1))) ||
		FAILED(Add_Event(L"BigCombo_Effects_Combo2", CRoland_Event_BigComboEffect::Create(this, pWrench_0_L_Matrix, 2))) ||
		FAILED(Add_Event(L"BigCombo_Effects_Combo3", CRoland_Event_BigComboEffect::Create(this, pWrench_2_L_Matrix, 3))) ||
		FAILED(Add_Event(L"BigSwipe_Effects", CRoland_Event_BigSwipeEffect::Create(this, CRoland_Event_BigSwipeEffect::SWIPE_START))) ||
		FAILED(Add_Event(L"BigSwipe_Effects_End", CRoland_Event_BigSwipeEffect::Create(this, CRoland_Event_BigSwipeEffect::SWIPE_END))) ||
		FAILED(Add_Event(L"BigSwipe_Effects_End_Hit", CRoland_Event_BigSwipeEffect::Create(this, CRoland_Event_BigSwipeEffect::SWIPE_HIT))))
		// BigSwipe_Effects_End_Hit
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Roland::Ready_Bounceable()
{
	list<CGameObject*> temp = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_InsteractionObject"));
	for (auto& pObj : temp) {
		IInteractive_Bounceable* p_Bounceable = dynamic_cast<IInteractive_Bounceable*>(pObj);

		if (p_Bounceable == nullptr)
			continue;

		_vector vSour = p_Bounceable->Get_OffsetPosition();
		_float fDest = XMVectorGetZ(vSour);
		if (fDest < 50.f) {
			m_BounceObject.push_back(pObj);
			m_BounceObjectPos.push_back(vSour);
		}
	}

	return S_OK;
}

HRESULT CBoss_Roland::Ready_PartObject()
{
	// UI
	{
		CUI_Boss::DESC BossDesc{};
		{
			BossDesc.pEnemyStats = &m_EnemyStats;
			BossDesc.pIsAggroed = &m_bIsAggroed;
			BossDesc.pIsDead = &m_bIsDead;

			BossDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
			BossDesc.pCenterMatrixPtr = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Hips");
			BossDesc.pIsUnblockable = &m_bIsUnblockable;

			lstrcpy(BossDesc.szName, m_szName);
		}
		if (FAILED(Add_PartObject(L"Part_UI_Boss", LEVEL_STATIC, L"Prototype_GameObject_UI_Boss", &BossDesc)))
			return E_FAIL;
	}
		
	// Part_Fist
	{
		CRoland_Fist::DESC FistDesc{};

		FistDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		FistDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Hand.L.2");
		FistDesc.pOwner = this;
		
		FistDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Fist", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Roland_Fist", &FistDesc)))
			return E_FAIL;
	}

	// Part_Belly
	{
		CRoland_Fist::DESC FistDesc{};

		FistDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		FistDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Torso");
		FistDesc.pOwner = this;

		FistDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Belly", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Roland_Belly", &FistDesc)))
			return E_FAIL;
	}

	// Part_Wrench_0_L
	{
		CRoland_Wrench_0_L::DESC WrenchDesc{};

		WrenchDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		WrenchDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Weapon.1.L");
		WrenchDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Wrench_0_L.data";
		WrenchDesc.pModel = m_pModelCom;
		WrenchDesc.pOwner = this;

		WrenchDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Wrench_0_L", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Roland_Wrench_0_L", &WrenchDesc)))
			return E_FAIL;
	}

	// Part_Wrench_1_L
	{
		CRoland_Wrench_1_L::DESC WrenchDesc{};

		WrenchDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		WrenchDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Weapon.2.L");
		WrenchDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Wrench_1_L.data";
		WrenchDesc.pModel = m_pModelCom;
		WrenchDesc.pOwner = this;

		WrenchDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Wrench_1_L", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Roland_Wrench_1_L", &WrenchDesc)))
			return E_FAIL;
	}

	// Part_Wrench_2_L
	{
		CRoland_Wrench_2_L::DESC WrenchDesc{};

		WrenchDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		WrenchDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Weapon.3.L");
		WrenchDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Wrench_2_L.data";
		WrenchDesc.pModel = m_pModelCom;
		WrenchDesc.pOwner = this;

		WrenchDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Wrench_2_L", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Roland_Wrench_2_L", &WrenchDesc)))
			return E_FAIL;
	}

	// Part_Wrench_0_R
	{
		CRoland_Wrench_0_R::DESC WrenchDesc{};

		WrenchDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		WrenchDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Weapon.1.R");
		WrenchDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Wrench_0_R.data";
		WrenchDesc.pModel = m_pModelCom;
		WrenchDesc.pOwner = this;

		WrenchDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Wrench_0_R", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Roland_Wrench_0_R", &WrenchDesc)))
			return E_FAIL;
	}

	// Part_Wrench_1_R
	{
		CRoland_Wrench_1_R::DESC WrenchDesc{};

		WrenchDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		WrenchDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Weapon.2.R");
		WrenchDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Wrench_1_R.data";
		WrenchDesc.pModel = m_pModelCom;
		WrenchDesc.pOwner = this;

		WrenchDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Wrench_1_R", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Roland_Wrench_1_R", &WrenchDesc)))
			return E_FAIL;
	}

	// Part_Wrench_2_R
	{
		CRoland_Wrench_2_R::DESC WrenchDesc{};

		WrenchDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		WrenchDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Weapon.3.R");
		WrenchDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Wrench_2_R.data";
		WrenchDesc.pModel = m_pModelCom;
		WrenchDesc.pOwner = this;

		WrenchDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Wrench_2_R", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Roland_Wrench_2_R", &WrenchDesc)))
			return E_FAIL;
	}	
	
	m_Wrenches.push_back(Find_PartObject({ L"Part_Wrench_1_R" }));
	m_Wrenches.push_back(Find_PartObject({ L"Part_Wrench_2_R" }));
	m_Wrenches.push_back(Find_PartObject({ L"Part_Wrench_0_L" }));
	m_Wrenches.push_back(Find_PartObject({ L"Part_Wrench_1_L" }));
	m_Wrenches.push_back(Find_PartObject({ L"Part_Wrench_2_L" }));

	m_WeaponColliders.push_back(static_cast<CCollider*>(Find_PartObject({ TEXT("Part_Wrench_0_R") })->Find_Component(TEXT("Com_Collider"))));
	m_WeaponColliders.push_back(static_cast<CCollider*>(Find_PartObject({ TEXT("Part_Wrench_0_R") })->Find_Component(TEXT("Com_BottomCollider"))));
	m_WeaponColliders.push_back(static_cast<CCollider*>(Find_PartObject({ TEXT("Part_Wrench_1_R") })->Find_Component(TEXT("Com_Collider"))));
	m_WeaponColliders.push_back(static_cast<CCollider*>(Find_PartObject({ TEXT("Part_Wrench_2_R") })->Find_Component(TEXT("Com_Collider"))));
	m_WeaponColliders.push_back(static_cast<CCollider*>(Find_PartObject({ TEXT("Part_Wrench_0_L") })->Find_Component(TEXT("Com_Collider"))));
	m_WeaponColliders.push_back(static_cast<CCollider*>(Find_PartObject({ TEXT("Part_Wrench_1_L") })->Find_Component(TEXT("Com_Collider"))));
	m_WeaponColliders.push_back(static_cast<CCollider*>(Find_PartObject({ TEXT("Part_Wrench_2_L") })->Find_Component(TEXT("Com_Collider"))));
	m_WeaponColliders.push_back(static_cast<CCollider*>(Find_PartObject({ TEXT("Part_Fist") })->Find_Component(TEXT("Com_Collider"))));
	
	return S_OK;
}

void CBoss_Roland::Locate(_fvector _vPos)
{
	Active();
	if (nullptr != m_pPhysicsControllerCom)
		m_pPhysicsControllerCom->Set_Position(_vPos);
	m_bIsAggroed = true;
}

void CBoss_Roland::Chase(_float fTimeDelta)
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

void CBoss_Roland::MoveSide(_float fTimeDelta)
{
	if (!m_bIsSideMove)
		return;

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Right(fTimeDelta * m_fSideMoveSpeed);
}


void CBoss_Roland::Select_Action()
{
	if (!m_bIsAggroed) {
		m_pFSMCom->Call_NextState(L"Wait");
		return;
	}

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 6) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CRoland_StateMachine::STATE_CLOSE);
	}
	else if (fDis < 12) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CRoland_StateMachine::STATE_RANGED);
	}
	else if (fDis < 35) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CRoland_StateMachine::STATE_OUTOFRANGE);
	}
	else {
		m_pFSMCom->Call_NextState(L"Wait");
		m_bIsAggroed = false;
	}
}

void CBoss_Roland::Delete_Physics()
{
	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}
}

_vector CBoss_Roland::Get_MinAngleTarget()
{	
	_vector vA = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vB = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVector4Normalize(vA - vB);
	_float ABx = XMVectorGetX(vB) - XMVectorGetX(vA);
	_float ABz = XMVectorGetZ(vB) - XMVectorGetZ(vA);

	_float ACx;
	_float ACz;

	CGameObject* pObj{nullptr};
	_float length = 0;
	_bool bInterrupted = m_pGameInstance->Ray_Cast_To_Collider_In_Layer(vA, vLook, 5.f, pObj, length, L"Layer_InsteractionObject");

	_float fCurTargetAngle = bInterrupted? 0.f : 180.f;
	_int iTargetIndex = -1;
	_vector vTargetPos{};
	for (size_t i = 0; i < m_BounceObjectPos.size(); i++)
	{
		if (i == m_iCurBouncingObjIndex)
			continue;

		ACx = XMVectorGetX(m_BounceObjectPos[i]._vector()) - XMVectorGetX(vA);
		ACz = XMVectorGetZ(m_BounceObjectPos[i]._vector()) - XMVectorGetZ(vA);

		_float dotProduct = ABx * ACx + ABz * ACz;

		_float magnitudeAB = sqrt(ABx * ABx + ABz * ABz);
		_float magnitudeAC = sqrt(ACx * ACx + ACz * ACz);
		_float cosTheta = dotProduct / (magnitudeAB * magnitudeAC);
		_float angleRad = acos(cosTheta);
		_float angleDeg = angleRad * (180.0f / 3.14159265f);

		if (!bInterrupted && abs(angleDeg) < fCurTargetAngle) {
			fCurTargetAngle = angleDeg;		
			iTargetIndex = (_int)i;
			vTargetPos = m_BounceObjectPos[i]._vector();
		}
		else if(bInterrupted && abs(angleDeg) > fCurTargetAngle){
			fCurTargetAngle = angleDeg;
			iTargetIndex = (_int)i;
			vTargetPos = m_BounceObjectPos[i]._vector();
		}
	}
	
	m_iCurBouncingObjIndex = iTargetIndex;	
	return vTargetPos;
}

void CBoss_Roland::Disable_AllWeaponCollider()
{
	for (auto& Col : m_WeaponColliders)
		Col->Set_Active(false);
}

void CBoss_Roland::Disable_AllWrenches()
{
	for (auto& Wrench : m_Wrenches)
		Wrench->Set_IsActive(false);
}

void CBoss_Roland::Enable_AllWrenches()
{
	for (auto& Wrench : m_Wrenches)	
		Wrench->Set_IsActive(true);	
}

void CBoss_Roland::Enable_RollEffects()
{
	// 루프 이펙트 호출 후 저장
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
	_vector vOffset = XMVectorSet(0.f, 1.25f, 0.f, 1.f);
	const _float4x4* pWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
	
	m_RollingEffects.push_back(m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ROLAND_ROLL_MESH_MAIN, vOffset, vLook, true, pWorldMatrix, 0.f, false));
	m_RollingEffects.push_back(m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ROLAND_ROLL_MESH_SMOKEWIND, vOffset, vLook, true, pWorldMatrix, 0.f, false));
}

void CBoss_Roland::Call_NonLoopRollEffects()
{
	// 디스토션, 링 이펙트 호출
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE::RIGHT);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
	_vector vOffset = XMVectorSet(0.f, 1.25f, 0.f, 1.f);
	_vector vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW).r[(_uint)CTransform::STATE::LOOK];
	const _float4x4* pWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vOffset + vCamLook, -vCamLook, true, pWorldMatrix, 0.f, false);

	_float fRand = CUtils::Compute_Random_Normal() - 0.5f;
	for (size_t i = 0; i < 3; i++)
	{
		_vector vWindOffset = vOffset + vRight * fRand;
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ROLAND_ROLL_MESH_WIND, vWindOffset, vLook, true, pWorldMatrix, 0.f, false);
	}

}

void CBoss_Roland::Disable_RollEffects()
{
	Call_NonLoopRollEffects();
	if (true == m_RollingEffects.empty())
		return;

	// 루프 종료, 저장한 이펙트 정리
	for (auto& Effect : m_RollingEffects)
		Effect->Clear_Info(0.f);
	m_RollingEffects.clear();
}

void CBoss_Roland::Rotate_RollEffects(_fvector vLookAt)
{
	if (true == m_RollingEffects.empty())
		return;
	// 튕겼을 때 호출
	for (auto& Effect : m_RollingEffects)
		static_cast<CTransform*>(Effect->Find_Component(g_strTransformTag))->LookAt(vLookAt);
}

void CBoss_Roland::Setup_Light()
{
	LIGHT_DESC LightDesc = {};

	_vector vLightPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vLightDir = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
	vLightPos = XMVectorSetY(vLightPos, (XMVectorGetY(vLightPos) + 0.1f));
	vLightPos = vLightPos + XMVectorScale(vLightDir, 2.f);
	LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.fRange = 10.f;
	XMStoreFloat4(&LightDesc.vPosition, vLightPos);
	LightDesc.vDiffuse = _float4(6.0f, 4.5f, 0.8f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
	//LightDesc.vSpecular = m_vSpecular;
	LightDesc.vAttenuation = _float3(1.f, 0.2461f, 0.5f);

	_wstring strLightTag = L"Light_Roland";
	CGameInstance::GetInstance()->Set_LightDesc(L"Light_Roland", LightDesc);
}


void CBoss_Roland::Respawn()
{
	if (m_bIsDead)
		return;

	if (FAILED(Ready_Physics()))
		return;

	_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	super::Respawn();
	_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	m_pPhysicsControllerCom->Set_Position(vCurPosition);

	m_bIsChase = false;
	m_fDissolveRate = 2.f;
	m_pColliderCom->Set_Active(true);
	m_pFSMCom->Call_NextState(L"Wait");
}

void CBoss_Roland::Death()
{
	Deactive();

	m_pColliderCom->Set_Active(false);
	super::Death();
}

CBoss_Roland* CBoss_Roland::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Roland* pInstance = new CBoss_Roland(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CBoss_Roland");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBoss_Roland::Clone(void* pArg)
{
	CBoss_Roland* pInstance = new CBoss_Roland(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CBoss_Roland");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Roland::Free()
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