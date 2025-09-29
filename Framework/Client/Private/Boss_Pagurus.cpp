#include "pch.h"
#include "Boss_Pagurus.h"
#include "GameInstance.h"
#include "IK_Manager.h"
#include "IK_Segment.h"

//Engine
#include "Movement.h"
#include "Bone.h"

//Map Trigger
#include "MapTrigger.h"

//States
#include "Pagurus_StateMachine.h"

// Events
#include "Enemy_Event_Collider.h"
#include "Pagurus_Event_ExecutionDamage.h"
#include "Pagurus_Event_GrabOff.h"
#include "Enemy_Event_Sound.h"

//PartObject
#include "UI_Boss.h"
#include "UI_Locked.h"
#include "CineCamera.h"
#include "Pagurus_Fork.h"	

CBoss_Pagurus::CBoss_Pagurus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBoss_Pagurus::CBoss_Pagurus(const CBoss_Pagurus& Prototype)
	: super(Prototype)
{
}

HRESULT CBoss_Pagurus::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CBoss_Pagurus::Initialize(void* pArg)
{
	CBoss_Pagurus::Pagurus_DESC* pDesc = static_cast<Pagurus_DESC*>(pArg);

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


	m_pModelCom->SetUp_Animation((_uint)PAGURUS_ANIM_INDEX::IDLE, true);
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));	
	
	m_pFSMCom->Call_NextState(L"Wait");
	return S_OK;
}	

void CBoss_Pagurus::Priority_Update(_float fTimeDelta)
{	
	m_pFSMCom->Priority_Update(this, fTimeDelta);	
	m_pMovementCom->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);	
}

void CBoss_Pagurus::Update(_float fTimeDelta)
{	
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	Detect_Player();

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

	m_pIKManager->Update(fTimeDelta);
	__super::Update(fTimeDelta);
}

void CBoss_Pagurus::Late_Update(_float fTimeDelta)
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


HRESULT CBoss_Pagurus::Render()
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

HRESULT CBoss_Pagurus::Render_Shadow()
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

void CBoss_Pagurus::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
}

void CBoss_Pagurus::On_Collision(CGameObject* pOther, _uint iGroup)
{

}

void CBoss_Pagurus::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{

}

void CBoss_Pagurus::Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager)
{

}

void CBoss_Pagurus::Parried()
{

}

void CBoss_Pagurus::Active()
{
	if (m_bIsDead)
		return;

	Ready_Physics();

	m_pMovementCom->Set_GravityScale(10.f);
	m_pMovementCom->Set_MaxFallingSpeed(10.f);

	super::Active();
}

void CBoss_Pagurus::Deactive()
{
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);

	Delete_Physics();	

	super::Deactive();
}

HRESULT CBoss_Pagurus::Ready_Components()
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
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Boss_Pagurus",
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
		CPagurus_StateMachine::IMKERTON_FSM_DESC Desc{};
		Desc.pFSMDesc.eMode = CFSM::FSM_RAND;
		Desc.pFSMDesc.pOwner = this;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Pagurus_StateMachine",
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

HRESULT CBoss_Pagurus::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fSlopeLimit = 0.1f;
		Desc.fRadius = 3.f;
		Desc.fHeight = 1.f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Pagurus::Ready_Events()
{
	CCollider* pCollider = static_cast<CPagurus_Fork*>(Find_PartObject({ L"Part_Fork" }))->Get_Collider();

	/* 콜라이더 이벤트 */
	if (FAILED(Add_Event(L"Fork_Col_On", CEnemy_Event_Collider::Create(this, pCollider, true))) ||
		FAILED(Add_Event(L"Fork_Col_Off", CEnemy_Event_Collider::Create(this, pCollider, false))))
		return E_FAIL;

	/* 공격 이벤트 */
	if (FAILED(Add_Event(L"Execution_Damage", CPagurus_Event_ExecutionDamage::Create(this, 1.f, 1.f))) ||
		FAILED(Add_Event(L"Execution_Eat_Damage", CPagurus_Event_ExecutionDamage::Create(this, 999.f, 999.f))) || 
		FAILED(Add_Event(L"GrabOff", CPagurus_Event_GrabOff::Create(this))))
		return E_FAIL;

	/* 사운드 이벤트 */
	{
		if (FAILED(Add_Event(L"Sound_Grab", CEnemy_Event_Sound::Create(this, L"Pagurus_Swoosh5.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_GrabSlam", CEnemy_Event_Sound::Create(this, L"Pagurus_KnifeSlam.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK2))) ||
			FAILED(Add_Event(L"Sound_Saw", CEnemy_Event_Sound::Create(this, L"Pagurus_Saw.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK1))) ||
			FAILED(Add_Event(L"Sound_Eat", CEnemy_Event_Sound::Create(this, L"Pagurus_Eat.wav", 0.7f, (_uint)SOUND_CHANNEL::ENEMY_ATTACK2))))
			return E_FAIL;
	}

	 
	////카메라 이벤트
	//if (FAILED(Add_Event(L"CineCamera_On", CInkerton_Event_CineCameraOn::Create(this))))
	//	return E_FAIL;

	//const _float4x4* pNutCrackerMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("HB_MixupHeavy.26");
	//const _float4x4* pGunMuzzleMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Gun_end.6");
	///* 이펙트 이벤트 */
	//if (FAILED(Add_Event(L"Charge_Effect", CInkerton_Event_Effect_ChargeAttack::Create(this, pNutCrackerMatrix))) ||
	//	FAILED(Add_Event(L"Heavy_Effect", CInkerton_Event_Effect_MixupHeavy::Create(this, pNutCrackerMatrix))) ||
	//	FAILED(Add_Event(L"Aim_Effect", CInkerton_Event_Effect_AimFire::Create(this, pGunMuzzleMatrix, false))) ||
	//	FAILED(Add_Event(L"Fire_Effect", CInkerton_Event_Effect_AimFire::Create(this, pGunMuzzleMatrix, true))) ||
	//	FAILED(Add_Event(L"Slash_Effect_Combo1", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 1))) ||
	//	FAILED(Add_Event(L"Slash_Effect_Combo2", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 2))) ||
	//	FAILED(Add_Event(L"Slash_Effect_Combo2_2", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 4))) ||
	//	FAILED(Add_Event(L"Slash_Effect_Combo3", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 3))) ||
	//	FAILED(Add_Event(L"Slash_Effect_LeftDodge4", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 5))) ||
	//	FAILED(Add_Event(L"Slash_Effect_MixupLight5", CInkerton_Event_Effect_Slash::Create(this, pNutCrackerMatrix, 6))) ||
	//	FAILED(Add_Event(L"NutCracker_Effect_Ready", CInkerton_Event_Effect_NutCracker::Create(this, pNutCrackerMatrix, true))) ||
	//	FAILED(Add_Event(L"NutCracker_Effect_Impact", CInkerton_Event_Effect_NutCracker::Create(this, pNutCrackerMatrix, false))) ||
	//	FAILED(Add_Event(L"Execution_Effect", CInkerton_Event_Effect_Execution::Create(this, pNutCrackerMatrix, false))) || 
	//	FAILED(Add_Event(L"Execution_Slam_Effect", CInkerton_Event_Effect_Execution::Create(this, pNutCrackerMatrix, true))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Pagurus::Ready_IK()
{
	CIK_Manager::IK_DESC IKDesc{};
	IKDesc.pOwner = this;
	IKDesc.iSegmentGroupSize = 8;
	IKDesc.fConstraintAngle = 60.f;
	IKDesc.fConstraintMaxDistance = 8.f;
	IKDesc.fConstraintMinDistance = 6.f;		
	IKDesc.fLerpSpeed = 6.f;
	IKDesc.fMaxHeight = 0.5f;
	IKDesc.pRootBoneTransform = m_pTransformCom;
	m_pIKManager = CIK_Manager::Create(&IKDesc);

	_int iGroupIndex = 0;
	{
		Set_Segment("Thigh_L_1", -70.f, 6.5f, iGroupIndex);
		Set_Segment("Knee_L_1", -70.f, 6.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_L_2", -90.f, 6.5f, iGroupIndex);
		Set_Segment("Knee_L_2", -90.f, 6.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_L_3", -110.f, 6.f, iGroupIndex);
		Set_Segment("Knee_L_3", -110.f, 6.f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_L_4", -115.f, 6.f, iGroupIndex);
		Set_Segment("Knee_L_4", -115.f, 6.f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_R_1", 70.f, 6.5f, iGroupIndex);
		Set_Segment("Knee_R_1", 70.f, 6.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_R_2", 90.f, 6.5f, iGroupIndex);
		Set_Segment("Knee_R_2", 90.f, 6.5f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_R_3", 110.f, 6.f, iGroupIndex);
		Set_Segment("Knee_R_3", 110.f, 6.f, iGroupIndex);
	} iGroupIndex++;
	{
		Set_Segment("Thigh_R_4", 115.f, 6.f, iGroupIndex);
		Set_Segment("Knee_R_4", 115.f, 6.f, iGroupIndex);
	}

	return S_OK;
}

void CBoss_Pagurus::Set_Segment(const _char* szBoneName, _float fBaseAngle, _float fDefaultDistance, _int _iGroupIndex)
{
	_int iBoneIndex;
	CIK_Segment::IK_Segment_DESC SegmentDesc{};

	vector<CBone*>* Bones = m_pModelCom->Get_Bones();

	iBoneIndex = m_pModelCom->Get_BoneIndex_By_Name(szBoneName);
	_int iParentBoneIndex = (*Bones)[iBoneIndex]->Get_ParentBoneIndex();

	SegmentDesc.fBaseAngle = fBaseAngle;	
	SegmentDesc.pCombinedMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name(szBoneName);	
	SegmentDesc.fPitchOffset = 90.f;
	SegmentDesc.fDefaultDistance = fDefaultDistance;
	SegmentDesc.pOwner = this;

	m_pIKManager->Add_SegmentGroup(_iGroupIndex, CIK_Segment::Create(&SegmentDesc));

}

HRESULT CBoss_Pagurus::Ready_PartObject()
{
	 /* Nutcracker */
	{
		CPagurus_Fork::DESC ForkDesc{};

		ForkDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		ForkDesc.pSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("ForkBendy_end");
		ForkDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_Fork", CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Pagurus_Fork", &ForkDesc)))
			return E_FAIL;	
	}

	// UI
	{
		//CUI_Boss::DESC BossDesc{};
		//{
		//	BossDesc.pEnemyStats = &m_EnemyStats;
		//	BossDesc.pIsAggroed = &m_bIsAggroed;
		//	BossDesc.pIsDead = &m_bIsDead;
		//	BossDesc.pIsGrab = &m_bIsGrab;

		//	BossDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		//	BossDesc.pCenterMatrixPtr = &m_pModelCom->Get_BoneCombinedMatrix_By_Name("Hip_x.55");

		//	lstrcpy(BossDesc.szName, m_szName);
		//}
		//if (FAILED(Add_PartObject(L"Part_UI_Boss", LEVEL_STATIC, L"Prototype_GameObject_UI_Boss", &BossDesc)))
		//	return E_FAIL;
	}

	// CineCamera
	{
		/*CCineCamera::DESC CineCameraDesc{};

		CineCameraDesc.CameraDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
		CineCameraDesc.CameraDesc.fNear = 0.1f;
		CineCameraDesc.CameraDesc.fFar = 1000.f;

		CineCameraDesc.strName = L"CineCamera_Inkerton";

		if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
			return E_FAIL;

		m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
		m_pCineCamera->Add_Cinematic(L"Execute", "../Bin/DataFiles/CameraData/Inkerton_Execute_Test.data");*/
	}

	return S_OK;
}

void CBoss_Pagurus::Ambush()
{
	if (m_bIsDead || m_isActive)
		return;

	m_isActive = true;
	m_bIsAggroed = true;	
}

void CBoss_Pagurus::Locate(_fvector _vPos)
{
	Active();
	if (nullptr != m_pPhysicsControllerCom)
		m_pPhysicsControllerCom->Set_Position(_vPos);
	m_bIsAggroed = true;
}

_bool CBoss_Pagurus::Detect_Player()
{
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	_float fX = XMVectorGetX(vPlayerPos);
	_float fY = XMVectorGetY(vPlayerPos);
	_float fZ = XMVectorGetZ(vPlayerPos);

	_float fArea = 40.f;
	if ((fX > 519.f - fArea && fX < 519.f + fArea && fZ > 864.f - fArea && fZ < 864.f + fArea && fY < 2.f)
		|| (fX > 410.f - fArea && fX < 410.f + fArea && fZ > 915.f - fArea && fZ < 915.f + fArea && fY < 2.f)) {

		if (!m_bIsAggroed) {			
			CGameInstance::GetInstance()->FadeSwap_Sound(L"Pagurus 280 bpm.wav", (_uint)SOUND_CHANNEL::BGM, 0.1f, .5f);
		}			

		m_bIsAggroed = true;
		return true;
	}

	if (m_bIsAggroed) {		
		CGameInstance::GetInstance()->FadeSwap_Sound(L"Field_BGM.wav", (_uint)SOUND_CHANNEL::BGM, 0.1f, .5f);
	}
	
	m_bIsAggroed = false;
	return false;
}

void CBoss_Pagurus::Delete_Physics()
{
	if (m_pPhysicsControllerCom != nullptr) {
		Delete_Component(TEXT("Com_PhysicsController"), reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom));
		Safe_Release(m_pPhysicsControllerCom);
	}
}

void CBoss_Pagurus::Chase(_float fTimeDelta)
{
	if (!m_bIsChase)
		return;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);	

	if (fDis < 2.f && m_fChaseSpeed > 0) {
		//m_bIsChase = false;
		m_pIKManager->Set_Operation(false);
		m_pIKManager->Reset_Position();
		return;
	}

	m_pIKManager->Set_Operation(true);
	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Straight(fTimeDelta * m_fChaseSpeed);
}

void CBoss_Pagurus::MoveSide(_float fTimeDelta)
{
	if (!m_bIsSideMove)
		return;

	m_pMovementCom->TurnToDirection(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION), fTimeDelta);
	m_pTransformCom->Go_Right(fTimeDelta * m_fSideMoveSpeed);
}

void CBoss_Pagurus::Select_Action()
{	
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);	
	_float fDis = Get_DistanceBtwTarget(vPlayerPos);

	_float fX = XMVectorGetX(vPlayerPos);
	_float fY = XMVectorGetY(vPlayerPos);
	_float fZ = XMVectorGetZ(vPlayerPos);

	_float fArea = 60.f;
	if (fDis < 4) {
		m_pFSMCom->Select_StatesFromGroup((_uint)CPagurus_StateMachine::STATE_CLOSE);
	}
	else if ((fX > 519.f - fArea && fX < 519.f + fArea && fZ > 864.f - fArea && fZ < 864.f + fArea && fY < 2.f)
		|| (fX > 410.f - fArea && fX < 410.f + fArea && fZ > 915.f - fArea && fZ < 915.f + fArea && fY < 2.f)) {
		m_pFSMCom->Call_NextState(L"Chase");
	}
	else {
		m_pFSMCom->Call_NextState(L"Patrol");		
	}
}

void CBoss_Pagurus::Respawn()
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

void CBoss_Pagurus::Death()
{
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Set_MaxFallingSpeed(0.f);
	Delete_Physics();
	
	m_pColliderCom->Set_Active(false);

	super::Death();
}

CBoss_Pagurus* CBoss_Pagurus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Pagurus* pInstance = new CBoss_Pagurus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CBoss_Pagurus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBoss_Pagurus::Clone(void* pArg)
{
	CBoss_Pagurus* pInstance = new CBoss_Pagurus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CBoss_Pagurus");
		Safe_Release(pInstance);
	}

	return pInstance;
}
 
void CBoss_Pagurus::Free()
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
