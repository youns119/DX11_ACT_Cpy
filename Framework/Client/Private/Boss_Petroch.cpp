#include "pch.h"
#include "Boss_Petroch.h"
#include "GameInstance.h"

#include "Player.h"
#include "Petroch_CrystalSpawner.h"
#include "Petroch_SharpCrystal.h"
#include "UI_Manager.h"

//Engine
#include "Collider.h"
#include "Movement.h"
#include "FSM.h"
#include "Texture.h"
#include "Physics_Controller.h"

//States
#include "Petroch_StateMachine.h"

// Events
#include "Enemy_Event_Sound.h"
#include "Enemy_Event_Collider.h"
#include "Petroch_Event_Move.h"
#include "Petroch_Event_MoveSide.h"
#include "Petroch_Event_GroundSlam.h"
#include "Petroch_Event_DivePunch.h"
#include "Petroch_Event_Pounce.h"
#include "Petroch_Event_Effect.h"

// Effects
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

//#include "Inkerton_Event_Camera.h"
//#include "Inkerton_Event_AnimSpeed.h"

//PartObject
#include "UI_Boss.h"
#include "UI_Locked.h"
#include "Petroch_Shell.h"
#include "Petroch_Blockable.h"
#include "Petroch_LeftClaw.h"


CBoss_Petroch::CBoss_Petroch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBoss_Petroch::CBoss_Petroch(const CBoss_Petroch& Prototype)
	: super(Prototype)
{
}

HRESULT CBoss_Petroch::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Petroch::Initialize(void* pArg)
{
	CBoss_Petroch::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	m_szName = L"거짓된 달 페트로크";
	
	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Physics()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	if (FAILED(Ready_CrystalSpawner()))
		return E_FAIL;

	if (FAILED(Ready_Events()))
		return E_FAIL;

	{/* PointLight */

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		vPos = XMVectorSetY(vPos, (XMVectorGetY(vPos) + 2.f));

		LIGHT_DESC LightDesc = {};

		LightDesc.eType = LIGHT_TYPE::POINT;
		LightDesc.fRange = 40.f;
		XMStoreFloat4(&LightDesc.vPosition, vPos);
		LightDesc.vDiffuse = _float4(5.4f, 2.f, 4.2f, 1.f);
		LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
		//LightDesc.vSpecular = m_vSpecular;
		LightDesc.vAttenuation = _float3(1.f, 0.2461f, 0.5f);

		_wstring strLightTag = L"Light_Petroch";
		m_pGameInstance->Add_Light(strLightTag, LightDesc);

	}

	m_fDissolveRate = 9.f;
	m_fDissolveDarknessRate = .7f;
	m_pFSMCom->Call_NextState(L"Hide");

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	
	return S_OK;
}

void CBoss_Petroch::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_pCrystals.size(); i++)
		m_pCrystals[i]->Priority_Update(fTimeDelta);

	for (size_t i = 0; i < m_pCrystalSpwners.size(); i++)
		m_pCrystalSpwners[i]->Priority_Update(fTimeDelta);

	m_pFSMCom->Priority_Update(this, fTimeDelta);
	m_pMovementCom->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}
	
void CBoss_Petroch::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_pCrystals.size(); i++)
		m_pCrystals[i]->Update(fTimeDelta);

	for (size_t i = 0; i < m_pCrystalSpwners.size(); i++)
		m_pCrystalSpwners[i]->Update(fTimeDelta);

	Dessolve(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);

	Decrease_BlockTimer(fTimeDelta);
	m_pShellHouseColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

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

void CBoss_Petroch::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_pCrystals.size(); i++)
		m_pCrystals[i]->Late_Update(fTimeDelta);

	for (size_t i = 0; i < m_pCrystalSpwners.size(); i++)
		m_pCrystalSpwners[i]->Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

	if (nullptr != m_pPhysicsControllerCom) {
		m_pGameInstance->Simulate_Physics(m_pPhysicsControllerCom);

#ifdef _DEBUG
		//m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
		//m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pShellHouseColliderCom);
		//m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsControllerCom);		
#endif // _DEBUG
	}

	__super::Late_Update(fTimeDelta);
}


HRESULT CBoss_Petroch::Render()
{
	for (size_t i = 0; i < m_pCrystals.size(); i++)
		m_pCrystals[i]->Render();

	for (size_t i = 0; i < m_pCrystalSpwners.size(); i++)
		m_pCrystalSpwners[i]->Render();

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

HRESULT CBoss_Petroch::Render_Shadow()
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

void CBoss_Petroch::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (!pPlayer)
		return;

	if (m_bIsHide)
	{
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Show_Interaction(L"들어가기");		
	}
}

void CBoss_Petroch::On_Collision(CGameObject* pOther, _uint iGroup)
{	
}

void CBoss_Petroch::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);	
	if (!pPlayer)
		return;

	if (m_bIsHide)
	{
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Hide_Interaction();		
	}
}

void CBoss_Petroch::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
{
	if (m_bIsDead)
		return;

	if (m_bIsBlock) {		
		return;
	}
	
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
		m_pFSMCom->Call_NextState(L"Capsize");
		Reset_Balance();
		m_EnemyStats.bIsCapsized = true;
		m_EnemyStats.bIsBalanceBreakable = false;
		return;
	}	

	CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Sword2.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 0.9f);
}

void CBoss_Petroch::Parried()
{
	Reset_RecoverDelay();
	m_bIsDamaged = true;
	if (m_bIsRoll)
	{
		m_pMovementCom->Set_RotateToDirection(true);
		_vector temp =  m_pTransformCom->Get_State(CTransform::STATE::LOOK);		
		//m_pTransformCom->Set_State(CTransform::STATE::LOOK, -temp);
		m_pMovementCom->Add_Velocity(-temp, 10000.1f);
		temp = XMVectorSetX(temp, 0.f);
		temp = XMVectorSetZ(temp, 0.f);
		m_pTransformCom->Rotate(-temp);

		m_pMovementCom->Set_RotateToDirection(false);
		static_cast<CPetroch_Shell*>(Find_PartObject({ L"Part_Shell" }))->Reposition_Shell();
		return;
	}

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

void CBoss_Petroch::Active()
{
	if (m_bIsDead)
		return;

	Ready_Physics();
	m_pMovementCom->Set_GravityScale(10.f);
	m_pMovementCom->Set_MaxFallingSpeed(10.f);

	super::Active();
}

void CBoss_Petroch::Deactive()
{
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);	
	Delete_Physics();

	if (m_bIsDead) {
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Show_TextUI(L"오염 구역 정화");
	}

	super::Deactive();
}

HRESULT CBoss_Petroch::Ready_Components()
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
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Boss_Petroch",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &tModelDesc)))
		return E_FAIL;

	if (m_pMovementCom == nullptr)
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 30.f;
		Desc.fMoveDecel = 20.f;
		Desc.fMaxSpeed = 15.f;
		Desc.fMaxFallingSpeed = 20.f;
		Desc.fRotationScale = 1.f;
		Desc.fGravityScale = 20.f;

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

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Petroch_StateMachine",
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

		m_pColliderCom->Set_Active(false);
	}

	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.fRadius = 2.5f;
		Desc.vCenter = { 0.f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MAP_INTERACTION;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_ShellHouseCollider", reinterpret_cast<CComponent**>(&m_pShellHouseColliderCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Petroch::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fScaleCoeff = 0.5f;
		Desc.fRadius = 2.f;
		Desc.fHeight = 0.3f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Petroch::Ready_Events()
{
	CCollider* pLeftClawCollider = static_cast<CPetroch_LeftClaw*>(Find_PartObject({ L"Part_LeftClaw" }))->Get_Collider();

	//이동 이벤트
	{
		if (FAILED(Add_Event(L"Move_Forward", CPetroch_Event_Move::Create(this, true, .8f))) ||
			FAILED(Add_Event(L"Move_Back", CPetroch_Event_Move::Create(this, true, -1.8f))) ||
			FAILED(Add_Event(L"Move_Look", CPetroch_Event_Move::Create(this, true, 0.f))) ||
			FAILED(Add_Event(L"Pounce_Move_Forward", CPetroch_Event_Move::Create(this, true, 3.f))) ||
			FAILED(Add_Event(L"Move_Stop", CPetroch_Event_Move::Create(this, false))) ||
			FAILED(Add_Event(L"Move_Right", CPetroch_Event_MoveSide::Create(this, true, 0.5f))) ||
			FAILED(Add_Event(L"Move_Left", CPetroch_Event_MoveSide::Create(this, true, -0.5f))) ||
			FAILED(Add_Event(L"SideMove_Stop", CPetroch_Event_MoveSide::Create(this, false))))
			return E_FAIL;
	}		   

	/* 콜라이더 이벤트 */
	{
		if (FAILED(Add_Event(L"LeftClaw_Col_On", CEnemy_Event_Collider::Create(this, pLeftClawCollider, true))) ||
			FAILED(Add_Event(L"LeftClaw_Col_Off", CEnemy_Event_Collider::Create(this, pLeftClawCollider, false))) ||
			FAILED(Add_Event(L"Body_Col_On", CEnemy_Event_Collider::Create(this, m_pColliderCom, true))) ||
			FAILED(Add_Event(L"Body_Col_Off", CEnemy_Event_Collider::Create(this, m_pColliderCom, false))))
			return E_FAIL;
	}

	/* 공격 이벤트 */
	{
		if (FAILED(Add_Event(L"Ground_Slam", CPetroch_Event_GroundSlam::Create(this))) ||
			FAILED(Add_Event(L"Pounce", CPetroch_Event_Pounce::Create(this))) ||
			FAILED(Add_Event(L"Dive_Punch", CPetroch_Event_DivePunch::Create(this))))
			return E_FAIL;
	}

	/* 사운드 이벤트 */
	{
		if (FAILED(Add_Event(L"Sound_Pounce", CEnemy_Event_Sound::Create(this, L"Petroch_Atk2.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))))
			return E_FAIL;
	}

	/* 이펙트 이벤트 */
	{
		if (FAILED(Add_Event(L"BackLegs_Charge", CPetroch_Event_Effect::Create(this, CPetroch_Event_Effect::CHARGE))) ||
			FAILED(Add_Event(L"BackLegs_GroundSlam", CPetroch_Event_Effect::Create(this, CPetroch_Event_Effect::GROUNDSLAM))) ||
			FAILED(Add_Event(L"Effect_JumpToDive", CPetroch_Event_Effect::Create(this, CPetroch_Event_Effect::JUMP))) ||
			FAILED(Add_Event(L"Effect_PounceAttack", CPetroch_Event_Effect::Create(this, CPetroch_Event_Effect::POUNCE))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Petroch::Ready_CrystalSpawner()	
{
	for (_int i = 0; i < 2; i++)
	{		
		CPetroch_CrystalSpawner::DESC CrystalSpawnerDesc{};
		CrystalSpawnerDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
		CrystalSpawnerDesc.pOwner = this;
		CPetroch_CrystalSpawner* pCrystalSpawner = static_cast<CPetroch_CrystalSpawner*>(m_pGameInstance->Clone_Prototype(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Petroch_CrystalSpawner", &CrystalSpawnerDesc));		

		pCrystalSpawner->Set_ParentWorldMatrix(&m_pTransformCom->Get_WorldMatrix_Ref());
		pCrystalSpawner->Set_IsActive(false);

		m_pCrystalSpwners.push_back(pCrystalSpawner);		
	}

	for (_int i = 0; i < m_iCrystalCount; i++)
	{
		CPetroch_SharpCrystal::DESC SharpCrystal{};
		SharpCrystal.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;

		CPetroch_SharpCrystal* pSharpCrystal = static_cast<CPetroch_SharpCrystal*>(m_pGameInstance->Clone_Prototype(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Petroch_SharpCrystal", &SharpCrystal));
		m_pCrystals.push_back(pSharpCrystal);
		pSharpCrystal->Set_PlaySound(true);
		pSharpCrystal->Set_IsActive(false);
	}

	return S_OK;
}

HRESULT CBoss_Petroch::Ready_PartObject()
{			
	// Left Claw
	{
		CPetroch_LeftClaw::DESC LeftClawDesc{};

		LeftClawDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		LeftClawDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("arm_l_002");
		LeftClawDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_LeftClaw", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Petroch_LeftClaw", &LeftClawDesc)))
			return E_FAIL;
	}

	//Shell
	{
		CPetroch_Shell::DESC ShellDesc{};

		ShellDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		ShellDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("ShellTransform");
		ShellDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Petroch_Shell.data";
		ShellDesc.pModel = m_pModelCom;
		ShellDesc.pOwner = this;

		ShellDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Shell", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Petroch_Shell", &ShellDesc)))
			return E_FAIL;
	}

	//Shell Blockalbe
	{
		CPetroch_Blockable::DESC ShellDesc{};

		ShellDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		ShellDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("ShellTransform");
		ShellDesc.szFilePath = "../Bin/DataFiles/PartObjectData/Petroch_Shell.data";
		ShellDesc.pModel = m_pModelCom;
		ShellDesc.pOwner = this;

		ShellDesc.PartObjectDesc.fSpeedPerSec = 10.f;

		if (FAILED(Add_PartObject(L"Part_Blockable", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Petroch_Blockable", &ShellDesc)))
			return E_FAIL;
	}


	//Prototype_GameObject_Petroch_Blockable
	// UI
	{
		CUI_Boss::DESC BossDesc{};
		{
			BossDesc.pEnemyStats = &m_EnemyStats;
			BossDesc.pIsAggroed = &m_bIsAggroed;
			BossDesc.pIsDead = &m_bIsDead;

			BossDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();

			BossDesc.pCenterMatrixPtr = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("torso");

			lstrcpy(BossDesc.szName, m_szName);
		}
		if (FAILED(Add_PartObject(L"Part_UI_Boss", LEVEL_STATIC, L"Prototype_GameObject_UI_Boss", &BossDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CBoss_Petroch::Dessolve(_float fTimeDelta)
{
	if (!m_bIsDessolve)
		return;

	m_fDissolveRate -= 0.015f;

	if (m_fDissolveRate < 1.8f && m_fDissolveRate > 0.5f)
		m_fSwellAmount += 0.008f;

	if (m_fDissolveRate < 0)
		Deactive();
}

void CBoss_Petroch::Set_Roll(_bool _bIsRoll)
{
	m_bIsRoll = _bIsRoll;

	if(m_pPhysicsControllerCom)
		m_pPhysicsControllerCom->Set_UserData(_bIsRoll);
}

void CBoss_Petroch::Decrease_BlockTimer(_float fTimeDelta)
{
	if (!m_bIsBlock)
		return;

	if (m_fCurBlockTimer > m_fMaxBlockTimer)
		m_bIsBlock = false;
	else
		m_fCurBlockTimer += fTimeDelta;
}

void CBoss_Petroch::Chase(_float fTimeDelta)
{
	if (!m_bIsChase)
		return;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 2.5f) {
		m_bIsChase = false;
		return;
	}

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Straight(fTimeDelta * m_fChaseSpeed);
}

void CBoss_Petroch::Delete_Physics()
{
	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}
}

void CBoss_Petroch::MoveSide(_float fTimeDelta)
{
	if (!m_bIsSideMove)
		return;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 2.f) {
		m_bIsChase = false;
		return;
	}

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Right(fTimeDelta * m_fSideMoveSpeed);
}

void CBoss_Petroch::Ground_Slam()
{
	// leg2_foot_l
	// leg2_foot_r

	_vector PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);

	_float4x4 LeftStartMat = m_pModelCom->Get_BoneCombinedMatrix_By_Name("leg2_foot_l");
	_float4x4 RightStartMat = m_pModelCom->Get_BoneCombinedMatrix_By_Name("leg2_foot_r");

	m_pCrystalSpwners[0]->Set_SpawnOption(&LeftStartMat, PlayerPos, 1.8f, 13.5f, 1.2f, true, 10);
	m_pCrystalSpwners[1]->Set_SpawnOption(&RightStartMat, PlayerPos, 1.8f, 13.5f, 1.2f, false, 10);
}

void CBoss_Petroch::Dive_Punch()
{
	// claw_1_r	
	_float4x4 StartMat = m_pModelCom->Get_BoneCombinedMatrix_By_Name("claw_1_r");

	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::LOOK));
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_float fDistance = 10.f;
	_vector vTarget = XMVectorAdd(vPosition, XMVectorScale(vLook, fDistance));

	m_pCrystalSpwners[0]->Set_SpawnOption(&StartMat, vTarget, 2.0f, 17.5f, 1.2f, true, 4);
	m_pCrystalSpwners[1]->Set_SpawnOption(&StartMat, vTarget, 2.0f, 17.5f, 1.2f, false, 4); 
}

void CBoss_Petroch::Pounce() 
{		
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::LOOK));
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_float fDistance = 5.f;
	_vector vTarget = XMVectorAdd(vPosition, XMVectorScale(vLook, fDistance));

	_matrix temp = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix_Ref());
	temp.r[(_uint)CTransform::STATE::POSITION] = vTarget;
	_float4x4 sour{};
	XMStoreFloat4x4(&sour, temp);
	m_pCrystals.at(m_iCurCrystalIndex)->Spawn(sour);
	m_iCurCrystalIndex++;
	m_iCurCrystalIndex %= m_pCrystals.size();
}

void CBoss_Petroch::Select_Action()
{
	if (!m_bIsAggroed) {
		m_pFSMCom->Call_NextState(L"Wait");
		return;
	}

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	if (fDis < 15) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CPetroch_StateMachine::STATE_CLOSE);
	}
	else if (fDis < 55) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CPetroch_StateMachine::STATE_RANGED);
	}
	else {
		m_pFSMCom->Select_StatesFromGroup((_uint)CPetroch_StateMachine::STATE_OUTOFRANGE);
	}
}

void CBoss_Petroch::Setup_Light()
{
	LIGHT_DESC LightDesc = {};

	_vector vLightPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	vLightPos = XMVectorSetY(vLightPos, (XMVectorGetY(vLightPos) + 1.5f));
	LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.fRange = 40.f;
	XMStoreFloat4(&LightDesc.vPosition, vLightPos);
	LightDesc.vDiffuse = _float4(5.4f, 2.f, 4.2f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
	//LightDesc.vSpecular = m_vSpecular;
	LightDesc.vAttenuation = _float3(1.f, 0.2461f, 0.5f);

	_wstring strLightTag = L"Light_Petroch";
	CGameInstance::GetInstance()->Set_LightDesc(L"Light_Petroch", LightDesc);
}

void CBoss_Petroch::Call_RollingEffects()
{
	if (false == m_RollingEffects.empty())
		Disable_RollingEffects();

	_vector vPos{}, vLook{}, vOffset{ XMVectorSet(0.f, 1.5f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	const _float4x4* pWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
	// 루프 이펙트 호출 후 저장
	vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
	m_RollingEffects.push_back(m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_MESH_ROLLING_LIGHT, vOffset, vLook, true, pWorldMatrix, 0.f, false));
	m_RollingEffects.push_back(m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_MESH_ROLLING_LINE, vOffset, vLook, true, pWorldMatrix, 0.f, false));
	m_RollingEffects.push_back(m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_MESH_ROLLING_DISTORTION, vOffset, vLook, true, pWorldMatrix, 0.f, false));
}

void CBoss_Petroch::Call_NonLoopRollingEffects()
{
	_vector vPos{}, vUp{}, vLook{}, vOffset{ XMVectorSet(0.f, -0.5f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
	vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);
	CEffectObject* pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_ROLLING_SANDBURST, vPos + vOffset, vZeroLook, false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vUp + vLook * 0.25f);
}

void CBoss_Petroch::Control_RollingEffects()
{
	if (true == m_RollingEffects.empty())
		return;
	
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
	for (auto& Effect : m_RollingEffects)
		static_cast<CTransform*>(Effect->Find_Component(g_strTransformTag))->LookAt(vLook);
}

void CBoss_Petroch::Disable_RollingEffects()
{
	if (true == m_RollingEffects.empty())
		return;

	// 루프 종료, 저장한 이펙트 정리
	for (auto& Effect : m_RollingEffects)
		Effect->Clear_Info(0.f);
	m_RollingEffects.clear();	
}

void CBoss_Petroch::Respawn()
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

void CBoss_Petroch::Death()
{
	m_pColliderCom->Set_Active(false);

	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);
	Delete_Physics();

	super::Death();
}

CBoss_Petroch* CBoss_Petroch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Petroch* pInstance = new CBoss_Petroch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CBoss_Petroch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBoss_Petroch::Clone(void* pArg)
{
	CBoss_Petroch* pInstance = new CBoss_Petroch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CBoss_Petroch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Petroch::Free()
{
	super::Free();

	m_pGameInstance->Remove_Light(L"Light_Petroch");

	for (auto& pObject : m_pCrystals)
		Safe_Release(pObject);

	for (auto& pObject : m_pCrystalSpwners)
		Safe_Release(pObject);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pFSMCom);
	Safe_Release(m_pMovementCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pPhysicsControllerCom);
	Safe_Release(m_pShellHouseColliderCom);
}
