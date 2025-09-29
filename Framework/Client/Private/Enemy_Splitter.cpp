#include "pch.h"
#include "Enemy_Splitter.h"
#include "GameInstance.h"

#include "UI_Manager.h"

//State Machine
#include "Splitter_StateMachine.h"

// Events
#include "Splitter_Event_Move.h"
#include "Splitter_Event_CineCameraOn.h"
#include "Splitter_Event_CineCameraShake.h"
#include "Enemy_Event_Collider.h"
#include "Enemy_Event_Sound.h"
#include "Splitter_Event_GrabOff.h"
#include "Splitter_Event_Fear.h"
#include "Splitter_Event_Effect_LockHit.h"
#include "Splitter_Event_Effect_LockOverhead.h"
#include "Splitter_Event_Effect_Grab.h"
#include "Splitter_Event_Effect_Execution.h"
#include "Splitter_Event_Effect_Bash.h"
#include "Splitter_Event_ExecutionDamage.h"

//PartObject
#include "UI_Boss.h"
#include "UI_Locked.h"
#include "Splitter_Weapon.h"
#include "Splitter_LeftClaw.h"

#include "CineCamera.h"

CEnemy_Splitter::CEnemy_Splitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEnemy_Splitter::CEnemy_Splitter(const CEnemy_Splitter& Prototype)
	: super(Prototype)
{	
}

HRESULT CEnemy_Splitter::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	// clawUpper_l.28_end

	// AllenWrench
	// AllenWrench.005
	return S_OK;
}

HRESULT CEnemy_Splitter::Initialize(void* pArg)
{
	CEnemy_Splitter::DESC* pDesc = static_cast<DESC*>(pArg);

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

	m_strName = TEXT("Splitter");

	m_pFSMCom->Call_NextState(L"Wait");
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	//XMStoreFloat4x4(&m_OriginalPos, m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}	

void CEnemy_Splitter::Priority_Update(_float fTimeDelta)
{			
	//250.f, 1.f, 355.f

	m_pMovementCom->Priority_Update(fTimeDelta);
	m_pFSMCom->Priority_Update(this, fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CEnemy_Splitter::Update(_float fTimeDelta)
{				
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());	

	//if (!m_bIsAggroed) {
	//	m_bIsAggroed = Detect_Target(230.f, 15.f, 15.f, m_pPlayerTransform, m_pTransformCom);
	//	if (m_bIsAggroed)
	//		m_pFSMCom->Call_NextState(L"Aggroed");
	//}

	// Movement
	{
		_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

		m_pFSMCom->Update(this, fTimeDelta);
		Chase(fTimeDelta);
		m_pMovementCom->Update(fTimeDelta);

		_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vec3 vDisp = vCurPosition - vPrePosition;
		if (m_pPhysicsControllerCom != nullptr)
			m_pPhysicsControllerCom->Move(vDisp, static_cast<_float>(fTimeDelta * 0.5), fTimeDelta);
	}

	//Control_GrayScale(fTimeDelta, 4.f);
	
	__super::Update(fTimeDelta);
}

void CEnemy_Splitter::Late_Update(_float fTimeDelta)
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

HRESULT CEnemy_Splitter::Render()
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

HRESULT CEnemy_Splitter::Render_Shadow()
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

void CEnemy_Splitter::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{	
}

void CEnemy_Splitter::On_Collision(CGameObject* pOther, _uint iGroup)
{	
}

void CEnemy_Splitter::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CEnemy_Splitter::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	CModel::DESC tModelDesc{};
	tModelDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Enemy_Splitter",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EnemyDessolveNoise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
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

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Splitter_StateMachine",
			L"Com_StateMachine", reinterpret_cast<CComponent**>(&m_pFSMCom), &Desc)))
			return E_FAIL;

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

HRESULT CEnemy_Splitter::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fScaleCoeff = 1.2f;
		Desc.fRadius = 1.0f;
		Desc.fHeight = 1.f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Splitter::Ready_Events()
{
	CCollider* pUpperWeapon = static_cast<CSplitter_Weapon*>(Find_PartObject({ L"Part_Weapon" }))->Get_UpperCollider();
	CCollider* pLowerWeapon = static_cast<CSplitter_Weapon*>(Find_PartObject({ L"Part_Weapon" }))->Get_LowerCollider();
	CCollider* pLeftClaw = static_cast<CSplitter_LeftClaw*>(Find_PartObject({ L"Part_LeftClaw" }))->Get_Collider();

	/* 이동 이벤트 */
	{
		if (FAILED(Add_Event(L"Swipe_Move", CSplitter_Event_Move::Create(this, true, 1.0f))) ||
			FAILED(Add_Event(L"Swipe_Stop", CSplitter_Event_Move::Create(this))) ||
			FAILED(Add_Event(L"Slam_Move", CSplitter_Event_Move::Create(this, true, 1.5f))) ||
			FAILED(Add_Event(L"Slam_Stop", CSplitter_Event_Move::Create(this))) ||
			FAILED(Add_Event(L"Bash_Move", CSplitter_Event_Move::Create(this, true, 1.1f))) ||
			FAILED(Add_Event(L"Bash_Stop", CSplitter_Event_Move::Create(this))) ||
			FAILED(Add_Event(L"Fear_Stop", CSplitter_Event_Move::Create(this))) ||
			FAILED(Add_Event(L"Hit_Look_0", CSplitter_Event_Move::Create(this, true, 0.f))) ||
			FAILED(Add_Event(L"Hit_Look_1", CSplitter_Event_Move::Create(this, true, 0.f))) ||
			FAILED(Add_Event(L"Hit_Look_2", CSplitter_Event_Move::Create(this, true, 0.f))) ||
			FAILED(Add_Event(L"Hit_Stop_0", CSplitter_Event_Move::Create(this))) ||
			FAILED(Add_Event(L"Hit_Stop_1", CSplitter_Event_Move::Create(this))) ||
			FAILED(Add_Event(L"Hit_Stop_2", CSplitter_Event_Move::Create(this))) ||
			FAILED(Add_Event(L"Look_Enter", CSplitter_Event_Move::Create(this, true, 0.f))) ||
			FAILED(Add_Event(L"Look_Exit", CSplitter_Event_Move::Create(this))))
			return E_FAIL;
	}

	/* 콜라이더 이벤트 */
	{
		if (FAILED(Add_Event(L"Bash_Col_On", CEnemy_Event_Collider::Create(this, pLeftClaw, true))) ||
			FAILED(Add_Event(L"Bash_Col_Off", CEnemy_Event_Collider::Create(this, pLeftClaw, false))) ||
			FAILED(Add_Event(L"Swipe_Col_On", CEnemy_Event_Collider::Create(this, pLeftClaw, true))) ||
			FAILED(Add_Event(L"Swipe_Col_Off", CEnemy_Event_Collider::Create(this, pLeftClaw, false))) ||
			FAILED(Add_Event(L"Slam_Col_On", CEnemy_Event_Collider::Create(this, pUpperWeapon, true))) ||
			FAILED(Add_Event(L"Slam_Col_On_1", CEnemy_Event_Collider::Create(this, pLowerWeapon, true))) ||
			FAILED(Add_Event(L"Slam_Col_Off", CEnemy_Event_Collider::Create(this, pUpperWeapon, false))) ||
			FAILED(Add_Event(L"Slam_Col_Off_1", CEnemy_Event_Collider::Create(this, pLowerWeapon, false))) ||
			FAILED(Add_Event(L"Hit_Col_On_0", CEnemy_Event_Collider::Create(this, pLowerWeapon, true))) ||
			FAILED(Add_Event(L"Hit_Col_On_1", CEnemy_Event_Collider::Create(this, pLowerWeapon, true))) ||
			FAILED(Add_Event(L"Hit_Col_On_2", CEnemy_Event_Collider::Create(this, pLowerWeapon, true))) ||
			FAILED(Add_Event(L"Hit_Col_Off_0", CEnemy_Event_Collider::Create(this, pLowerWeapon, false))) ||
			FAILED(Add_Event(L"Hit_Col_Off_1", CEnemy_Event_Collider::Create(this, pLowerWeapon, false))) ||
			FAILED(Add_Event(L"Hit_Col_Off_2", CEnemy_Event_Collider::Create(this, pLowerWeapon, false))))
			return E_FAIL;
	}

	// 처형 이벤트
	{
		if (FAILED(Add_Event(L"CineCamera_On", CSplitter_Event_CineCameraOn::Create(this))) ||
			FAILED(Add_Event(L"CineCamera_Shake", CSplitter_Event_CineCameraShake::Create(this))) ||
			FAILED(Add_Event(L"Grab_Off", CSplitter_Event_GrabOff::Create(this))) ||
			FAILED(Add_Event(L"Execution_Damage", CSplitter_Event_ExecutionDamage::Create(this, 10.f))))
			return E_FAIL;		
	}

	/* 사운드 이벤트 */
	{
		if (FAILED(Add_Event(L"Sound_Hit_0", CEnemy_Event_Sound::Create(this, L"Executioner Slam 1.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Hit_1", CEnemy_Event_Sound::Create(this, L"Executioner Slam 2.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Hit_2", CEnemy_Event_Sound::Create(this, L"Executioner Slam 3.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Slam", CEnemy_Event_Sound::Create(this, L"Executioner Overhead Swing.wav", 1.f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_ExecutionSlam", CEnemy_Event_Sound::Create(this, L"Executioner BeefUp.wav", 0.8f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_ExecutionGuillo", CEnemy_Event_Sound::Create(this, L"Executioner Grab Execute.wav", 0.8f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Fear", CEnemy_Event_Sound::Create(this, L"Executioner_FearV3.wav", 0.8f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))))
			return E_FAIL;
	}
	
	// 이펙트 이벤트
	{
		// 공포
		if (FAILED(Add_Event(L"Event_Effect_Splitter_Fear", CSplitter_Event_Fear::Create(this))))
			return E_FAIL;

		// 자물쇠 내려찍기
		if (FAILED(Add_Event(L"Hit_Effect_Lock", CSplitter_Event_Effect_LockHit::Create(this))))
			return E_FAIL;

		// 자물쇠 슬램
		if (FAILED(Add_Event(L"Slam_Effect", CSplitter_Event_Effect_LockOverhead::Create(this))))
			return E_FAIL;

		// 스와이프
		if (FAILED(Add_Event(L"Swipe_Effect", CSplitter_Event_Effect_Grab::Create(this))))
			return E_FAIL;

		// 처형
		if (FAILED(Add_Event(L"Execution_Effect", CSplitter_Event_Effect_Execution::Create(this))))
			return E_FAIL;

		// 대쉬
		if (FAILED(Add_Event(L"Bash_Effect", CSplitter_Event_Effect_Bash::Create(this))))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CEnemy_Splitter::Ready_PartObject()
{
	// Weapon
	{
		CSplitter_Weapon::DESC WeaponDesc{};

		WeaponDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();		
		WeaponDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Lock.63");		
		WeaponDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_Weapon", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Splitter_Weapon", &WeaponDesc)))
			return E_FAIL;
	}

	// Left Claw
	{
		CSplitter_LeftClaw::DESC LeftClawDesc{};

		LeftClawDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		LeftClawDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("GrabTransform.36");
		LeftClawDesc.pOwner = this;

		//GrabTransform.36
		//clawlower_l.27

		if (FAILED(Add_PartObject(L"Part_LeftClaw", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Splitter_LeftClaw", &LeftClawDesc)))
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
			BossDesc.pCenterMatrixPtr = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("spine.12");
			BossDesc.pIsUnblockable = &m_bIsUnblockable;

			m_szName = L"왕실 쉘스플리터";
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

		CineCameraDesc.strName = L"CineCamera_Splitter";

		if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
			return E_FAIL;

		m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
		m_pCineCamera->Add_Cinematic(L"Execute", "../Bin/DataFiles/CameraData/Splitter_Execute.data");
	}

	return S_OK;
}

void CEnemy_Splitter::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
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
		m_pGameInstance->FadeSwap_Sound(L"Field_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 2.f, 0.5f);

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

	CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Sword3.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.7f);
}

void CEnemy_Splitter::Parried()
{
	Reset_RecoverDelay();
	m_bIsDamaged = true;
	m_EnemyStats.fCurBP += 10.f;

	if (m_EnemyStats.fCurBP >= m_EnemyStats.fMaxBP) {
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

void CEnemy_Splitter::Active()
{	
	if (m_bIsDead)
		return;

	Ready_Physics();

	m_pMovementCom->Set_GravityScale(10.f);
	m_pMovementCom->Set_MaxFallingSpeed(10.f);

	super::Active();
}

void CEnemy_Splitter::Deactive()
{	
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);

	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}
	
	if (m_bIsDead)
	{
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Show_TextUI(L"오염 구역 정화");
	}

	super::Deactive();
}

void CEnemy_Splitter::Respawn()
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
	m_pColliderCom->Set_Active(true);
	m_pFSMCom->Call_NextState(L"Wait");
}

void CEnemy_Splitter::Death()
{	
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);

	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}

	m_pColliderCom->Set_Active(false);

	super::Death();	
}

void CEnemy_Splitter::Locate(_fvector _vPos)
{	
	Active();
	if(nullptr != m_pPhysicsControllerCom)
		m_pPhysicsControllerCom->Set_Position(_vPos);

	m_OriginalPos._41 = XMVectorGetX(_vPos);
	m_OriginalPos._42 = XMVectorGetY(_vPos);
	m_OriginalPos._43 = XMVectorGetZ(_vPos);

	m_bIsAggroed = true;
}

void CEnemy_Splitter::Chase(_float fTimeDelta)
{
	if (!m_bIsChase || m_pMovementCom == nullptr)
		return;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);
	 
	if (fDis < 1.5f) {
		m_bIsChase = false;
		return;
	}

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Straight(fTimeDelta * m_fChaseSpeed);
}

void CEnemy_Splitter::Select_Action()
{
	if (!m_bIsAggroed) {
		if (m_PatrolPoints.size() > 0)
			m_pFSMCom->Call_NextState(L"Patrol");
		else
			m_pFSMCom->Select_StatesFromGroup((_uint)CSplitter_StateMachine::STATE_UNAGGRO);
		return;
	}

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 5) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CSplitter_StateMachine::STATE_CLOSE);
	}
	else if (fDis < 12) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CSplitter_StateMachine::STATE_RANGED);
	}
	else if (fDis < 20) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CSplitter_StateMachine::STATE_FAR);
	}
	else {
		m_pFSMCom->Select_StatesFromGroup((_uint)CSplitter_StateMachine::STATE_OUTOFRANGE);
	}
}

void CEnemy_Splitter::Control_GrayScale(_float fTimeDelta, _float fSpeed)
{
	_float fCurSaturation = m_pGameInstance->Get_ColorCorrectionParams().fSaturation;
	if (true == m_bIsGrayScaling)
	{
		if (fCurSaturation > 0.f)
		{
			COLORCORRECTION_PARAMS param{};
			param.fSaturation = fCurSaturation - fTimeDelta * fSpeed;
			m_pGameInstance->Set_ColorCorrectionParams(param);
		}
	}
	else
	{
		if (fCurSaturation < 1.f)
		{
			COLORCORRECTION_PARAMS param {};
			param.fSaturation = fCurSaturation + fTimeDelta * fSpeed;
			m_pGameInstance->Set_ColorCorrectionParams(param);
		}
	}
}

CEnemy_Splitter* CEnemy_Splitter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Splitter* pInstance = new CEnemy_Splitter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CEnemy_Spliter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CEnemy_Splitter::Clone(void* pArg)
{
	CEnemy_Splitter* pInstance = new CEnemy_Splitter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEnemy_Spliter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Splitter::Free()
{
	super::Free();
		
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pFSMCom);
	Safe_Release(m_pMovementCom);
	Safe_Release(m_pNoiseTextureCom);	
	Safe_Release(m_pPhysicsControllerCom);
	
}