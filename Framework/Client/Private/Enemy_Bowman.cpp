#include "pch.h"
#include "Enemy_Bowman.h"
#include "GameInstance.h"

//IK
#include "IK_Manager.h"
#include "IK_Segment.h"
#include "Bone.h"

//States
#include "Bowman_StateMachine.h"

//Part Object
#include "Bowman_RightClaw.h"
#include "Bowman_Arrow.h"
#include "UI_Monster.h"

//Events
#include "Enemy_Event_Collider.h"
#include "Enemy_Event_Sound.h"
#include "Bowman_Event_Move.h"
#include "Bowman_Event_Archery.h"
#include "Bowman_Event_Reload.h"
#include "Bowman_Event_Effect.h"

CEnemy_Bowman::CEnemy_Bowman(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEnemy_Bowman::CEnemy_Bowman(const CEnemy_Bowman& Prototype)
	: super(Prototype)
{
}

HRESULT CEnemy_Bowman::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Bowman::Initialize(void* pArg)
{
	CEnemy_Bowman::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Physics()))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	if (FAILED(Ready_Events()))
		return E_FAIL;

	if (FAILED(Ready_IK()))
		return E_FAIL;

	// 화살
	// Arrow.75

	m_szName = L"Bowman";
	m_pFSMCom->Call_NextState(L"Wait");
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CEnemy_Bowman::Priority_Update(_float fTimeDelta)
{	
	m_pFSMCom->Priority_Update(this, fTimeDelta);
	
	__super::Priority_Update(fTimeDelta);
}

void CEnemy_Bowman::Update(_float fTimeDelta)
{			
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsAggroed)
		m_bIsAggroed = Detect_Target(140.f, 20.f, 19.f, m_pPlayerTransform, m_pTransformCom);

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

	__super::Update(fTimeDelta);
}

void CEnemy_Bowman::Late_Update(_float fTimeDelta)
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

HRESULT CEnemy_Bowman::Render()
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

		if (m_bIsDessolve)
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::DISSOLVE;

		m_pShaderCom->Begin(iShaderPass);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEnemy_Bowman::Render_Shadow()
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

void CEnemy_Bowman::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy_Bowman::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy_Bowman::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy_Bowman::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
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

	if (m_EnemyStats.bIsBalanceBreakable && eDamage == IDamaged::DAMAGE::DAMAGE_HEAVY) {
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

void CEnemy_Bowman::Parried()
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

void CEnemy_Bowman::Active()
{
	if (m_bIsDead)
		return;
	
	Ready_Physics();	
	super::Active();
	m_pIKManager->Reset_Position();
}

void CEnemy_Bowman::Deactive()
{
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);

	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}	

	super::Deactive();
}

void CEnemy_Bowman::Respawn()
{
	if (FAILED(Ready_Physics()))
		return;

	m_pIKManager->Set_Operation(true);

	_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	super::Respawn();
	_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vec3 vDisp = vCurPosition - vPrePosition;
	m_pPhysicsControllerCom->Move(vDisp, static_cast<_float>(0.f), 0.f);

	m_pColliderCom->Set_Active(true);
	m_pFSMCom->Call_NextState(L"Wait");
}

void CEnemy_Bowman::Death()
{
	m_pIKManager->Set_Operation(false);
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

void CEnemy_Bowman::Chase(_float fTimeDelta)
{
	if (!m_bIsChase)
		return;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 2.5f) {		
		return;
	}

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Straight(fTimeDelta * m_fChaseSpeed);
}


HRESULT CEnemy_Bowman::Ready_Components()
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
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Enemy_Bowman",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;

	/* Com_StateMachine */
	{
		CFSM::FSM_DESC Desc{};
		Desc.eMode = CFSM::FSM_RAND;
		Desc.pOwner = this;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Bowman_StateMachine",
			L"Com_StateMachine", reinterpret_cast<CComponent**>(&m_pFSMCom), &Desc)))
			return E_FAIL;
	}

	if (m_pMovementCom == nullptr)
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 50.f;
		Desc.fMoveDecel = 50.f;
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
		Desc.fRadius = 1.5f;
		Desc.vCenter = { 0.f, 1.5f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);
	}

	return S_OK;
}

HRESULT CEnemy_Bowman::Ready_Physics()
{
	m_pMovementCom->Set_GravityScale(10.f);
	m_pMovementCom->Set_MaxFallingSpeed(10.f);

	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fRadius = 1.0f;
		Desc.fHeight = 0.3f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Bowman::Ready_Events()
{
	CCollider* pArrowCollider = static_cast<CBowman_Arrow*>(Find_PartObject({ L"Part_Arrow" }))->Get_Collider();
	CCollider* pClawCollider = static_cast<CBowman_RightClaw*>(Find_PartObject({ L"Part_RightClaw" }))->Get_Collider();
	
	/* 콜라이더 이벤트 */
	{
		if (FAILED(Add_Event(L"Bash_Col_On", CEnemy_Event_Collider::Create(this, pClawCollider, true))) ||
			FAILED(Add_Event(L"Bash_Col_Off", CEnemy_Event_Collider::Create(this, pClawCollider, false))) ||
			FAILED(Add_Event(L"Stab_Col_On_0", CEnemy_Event_Collider::Create(this, pArrowCollider, true))) ||
			FAILED(Add_Event(L"Stab_Col_On_1", CEnemy_Event_Collider::Create(this, pArrowCollider, true))) ||
			FAILED(Add_Event(L"Stab_Col_On_2", CEnemy_Event_Collider::Create(this, pArrowCollider, true))) ||
			FAILED(Add_Event(L"Stab_Col_Off_0", CEnemy_Event_Collider::Create(this, pArrowCollider, false))) ||
			FAILED(Add_Event(L"Stab_Col_Off_1", CEnemy_Event_Collider::Create(this, pArrowCollider, false))) ||
			FAILED(Add_Event(L"Stab_Col_Off_2", CEnemy_Event_Collider::Create(this, pArrowCollider, false))) ||
			FAILED(Add_Event(L"Snap_Col_On", CEnemy_Event_Collider::Create(this, pClawCollider, true))) ||
			FAILED(Add_Event(L"Snap_Col_Off", CEnemy_Event_Collider::Create(this, pClawCollider, false))) ||
			FAILED(Add_Event(L"Fire_Arrow", CBowman_Event_Archery::Create(this))) ||
			FAILED(Add_Event(L"Reload_Arrow", CBowman_Event_Reload::Create(this))))
			return E_FAIL;
	}
	
	/* 사운드 이벤트 */
	{
		if (FAILED(Add_Event(L"Soud_FireArrow", CEnemy_Event_Sound::Create(this, L"Bowman Fire 1 (temporary).wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Soud_Bash", CEnemy_Event_Sound::Create(this, L"Bowman Claw Bash Impact.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Soud_Stab_0", CEnemy_Event_Sound::Create(this, L"Bowman Stab 1.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Soud_Stab_1", CEnemy_Event_Sound::Create(this, L"Bowman Stab 2.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Soud_Stab_2", CEnemy_Event_Sound::Create(this, L"Bowman Stab 3.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Snap", CEnemy_Event_Sound::Create(this, L"Bowman Snap Impact.wav", 0.5f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK2))))
			return E_FAIL;
	}

	/* 이동 이벤트 */
	{
		if (FAILED(Add_Event(L"Move_Backward", CBowman_Event_Move::Create(this, true, -1.f))) ||
			FAILED(Add_Event(L"Move_Forward", CBowman_Event_Move::Create(this, true, 0.5f))) ||
			FAILED(Add_Event(L"Move_Stop", CBowman_Event_Move::Create(this, false))))
			return E_FAIL;
	}

	/* 이펙트 이벤트 */
	{
		if (FAILED(Add_Event(L"Effect_Bowman_Jump", CBowman_Event_Effect::Create(this, CBowman_Event_Effect::BOWMAN_EFFECT::JUMP))) ||
			FAILED(Add_Event(L"Effect_Bowman_Distortion", CBowman_Event_Effect::Create(this, CBowman_Event_Effect::BOWMAN_EFFECT::DISTORTION))) ||
			FAILED(Add_Event(L"Effect_Bowman_Bash", CBowman_Event_Effect::Create(this, CBowman_Event_Effect::BOWMAN_EFFECT::BASH))) ||
			FAILED(Add_Event(L"Effect_Bowman_Claw", CBowman_Event_Effect::Create(this, CBowman_Event_Effect::BOWMAN_EFFECT::CLAW))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Bowman::Ready_PartObject()
{	
	// Right Claw
	{
		CBowman_RightClaw::DESC RightClawDesc{};

		RightClawDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		RightClawDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("RubberBand1_big_002.35");
		RightClawDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_RightClaw", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Bowman_RightClaw", &RightClawDesc)))
			return E_FAIL;
	}

	// Arrow
	{
		CBowman_Arrow::DESC ArrowDesc{};

		ArrowDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		ArrowDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("ClawGripPoint_L.22");
		ArrowDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Bowman_Arrow.data";
		ArrowDesc.pModel = m_pModelCom;
		ArrowDesc.pOwner = this;

		ArrowDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Arrow", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Bowman_FakeArrow", &ArrowDesc)))
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

HRESULT CEnemy_Bowman::Ready_IK()
{
	CIK_Manager::IK_DESC IKDesc{};
	IKDesc.pOwner = this;
	IKDesc.iSegmentGroupSize = 6;
	IKDesc.fConstraintAngle = 25.f;
	IKDesc.fConstraintMinDistance = 0.5f;
	IKDesc.fConstraintMaxDistance = 2.f;
	//IKDesc.fDefaultDistance = 1.6f;
	IKDesc.pRootBoneTransform = m_pTransformCom;	
	IKDesc.fLerpSpeed = 5.8f;
	IKDesc.fMaxHeight = 0.05f;

	m_pIKManager = CIK_Manager::Create(&IKDesc);

	_int iGroupIndex = 0;
	{		
		Set_Segment("Thigh_L_1.45", -70.f, 1.7f, iGroupIndex);
		Set_Segment("Knee_L_1.46", -70.f, 1.7f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_L_2.48", -90.f, 1.7f, iGroupIndex);
		Set_Segment("Knee_L_2.49", -90.f, 1.7f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_L_3.51", -120.f, 1.7f, iGroupIndex);
		Set_Segment("Knee_L_3.52", -120.f, 1.7f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_R_1.54", 70.f, 1.7f, iGroupIndex);
		Set_Segment("Knee_R_1.55", 70.f, 1.7f, iGroupIndex);
	} iGroupIndex++;
	{		
		Set_Segment("Thigh_R_2.57", 90.f, 1.7f, iGroupIndex);
		Set_Segment("Knee_R_2.58", 90.f, 1.7f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_R_3.60", 120.f, 1.7f, iGroupIndex);
		Set_Segment("Knee_R_3.61", 120.f, 1.7f, iGroupIndex);
	}

	return S_OK;
}

void CEnemy_Bowman::Set_Segment(const _char* szBoneName, _float fBaseAngle, _float fDefaultDistance, _int _iGroupIndex)
{	
	CIK_Segment::IK_Segment_DESC SegmentDesc{};		
	SegmentDesc.pOwner = this;
	SegmentDesc.fBaseAngle = fBaseAngle;		
	SegmentDesc.fPitchOffset = 89.f;	
	SegmentDesc.fDefaultDistance = fDefaultDistance;
	SegmentDesc.pCombinedMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name(szBoneName);

	m_pIKManager->Add_SegmentGroup(_iGroupIndex, CIK_Segment::Create(&SegmentDesc));
}


void CEnemy_Bowman::Select_Action()
{
	if (!m_bIsAggroed) {
		m_pFSMCom->Call_NextState(L"Wait");
		return;
	}

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 10) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CBowman_StateMachine::STATE_CLOSE);
	}
	else if (fDis < 40) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CBowman_StateMachine::STATE_RANGED);
	}
	else {
		m_pFSMCom->Select_StatesFromGroup((_uint)CBowman_StateMachine::STATE_OUTOFRANGE);
	}
}

CEnemy_Bowman* CEnemy_Bowman::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Bowman* pInstance = new CEnemy_Bowman(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CEnemy_Bowman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CEnemy_Bowman::Clone(void* pArg)
{
	CEnemy_Bowman* pInstance = new CEnemy_Bowman(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEnemy_Bowman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Bowman::Free()
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