#include "pch.h"
#include "Roland_State_Roll.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"

#include "UI_Manager.h"+
#include "Roland_Wrench_0_R.h"


CRoland_State_Roll::CRoland_State_Roll()
{
}

HRESULT CRoland_State_Roll::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Roll";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;
	m_pRoland = static_cast<CBoss_Roland*>(pObj);
	m_pModel = static_cast<CModel*>(m_pRoland->Find_Component(TEXT("Com_Model")));	
	m_pTransform = static_cast<CTransform*>(m_pRoland->Find_Component(TEXT("Com_Transform")));
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pMovement = static_cast<CMovement*>(m_pRoland->Find_Component(TEXT("Com_Movement")));
	return S_OK;
}

void CRoland_State_Roll::Enter(void* pObj)
{			
	static_cast<CRoland_Wrench_0_R*>(m_pRoland->Find_PartObject({ TEXT("Part_Wrench_0_R") }))->Set_IsActive(false);
	m_pRoland->Set_Roll(true);
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::ROLLUP, false, 0.2f);
	m_fCurPreRollTime = 0.f;
	m_iCurAnimIndex = (_uint)ROLAND_ANIM_INDEX::ROLLUP;
	m_bIsFirstBounce = true;
	

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	pUIManager->Hide_Caption();

	CGameInstance::GetInstance()->Play_Sound(L"Roland_RollEnter.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	
	pUIManager->Show_Caption(L"ÀÚ, ÃµµÕ ¸À Á» ºÁ¶ó!", 3.f);

	super::Enter(pObj);
}

void CRoland_State_Roll::Update(_float fTimeDelta, void* pObj)
{
	if (m_iCurAnimIndex == (_uint)ROLAND_ANIM_INDEX::ROLLUP && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::ROLLUP)) {
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::ROLL, true, 0.2f);		
		m_vRollTargetPos = m_pRoland->Get_MinAngleTarget();
		m_iCurAnimIndex = (_uint)ROLAND_ANIM_INDEX::ROLL;
		CGameInstance::GetInstance()->Play_Sound(L"Roland_RollCharge.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 0.5f);
		CGameInstance::GetInstance()->Play_Sound_Loop(L"Roland_Roll.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK2, 0.5);

		m_pRoland->Enable_RollEffects();
	}	
	else if (m_iCurAnimIndex == (_uint)ROLAND_ANIM_INDEX::STUNNED_UNROLL && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::STUNNED_UNROLL)) {
		static_cast<CRoland_Wrench_0_R*>(m_pRoland->Find_PartObject({ TEXT("Part_Wrench_0_R") }))->Set_IsActive(true);		
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::STUNNED_EXIT, false, 0.2f);
		m_iCurAnimIndex = (_uint)ROLAND_ANIM_INDEX::STUNNED_EXIT;		
	}
	else if (m_iCurAnimIndex == (_uint)ROLAND_ANIM_INDEX::STUNNED_EXIT && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::STUNNED_EXIT)) {
		m_pRoland->Select_Action();
	}
	else if (m_iCurAnimIndex == (_uint)ROLAND_ANIM_INDEX::UNROLL_INTO_BELLY_FLOP && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::UNROLL_INTO_BELLY_FLOP)) {
		static_cast<CRoland_Wrench_0_R*>(m_pRoland->Find_PartObject({ TEXT("Part_Wrench_0_R") }))->Set_IsActive(true);
		m_bIsFlop = false;
		m_pTransform->Set_Speed(1.f);
		m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.f);
		m_pRoland->Select_Action();
	}

	if (m_iCurAnimIndex == (_uint)ROLAND_ANIM_INDEX::ROLL) {
		Roll(fTimeDelta);
	}

	if (m_iCurAnimIndex == (_uint)ROLAND_ANIM_INDEX::UNROLL_INTO_BELLY_FLOP && m_bIsFlop) {
		Belly_Flop(fTimeDelta);
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_Roll::Exit(void* pObj)
{
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_float dest = XMVectorGetY(m_pTransform->Get_State(CTransform::STATE::POSITION));
	vPlayerPos = XMVectorSetY(vPlayerPos, dest);
	m_pTransform->LookAt(vPlayerPos);
	m_pRoland->Set_Roll(false);	
	m_pRoland->Set_Bounced(false);
	super::Exit(pObj);
}

void CRoland_State_Roll::Set_BellyFlopSpeed()
{	
	_vector vCurPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vCurPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);

	_float dx = XMVectorGetX(vCurPlayerPos) - XMVectorGetX(vCurPos);
	_float dy = XMVectorGetY(vCurPlayerPos) - XMVectorGetY(vCurPos);
	_float dz = XMVectorGetZ(vCurPlayerPos) - XMVectorGetZ(vCurPos);

	_float fDis = sqrt(dx * dx + dy * dy + dz * dz);


	_float fCurFrame = (_float)m_pModel->Get_CurrAnimation()->Get_TrackPosition();
	_float fCurAnimSpeed = m_pModel->Get_CurrAnimation()->Get_BaseAnimSpeed();
	_float fTickPerSec = (_float)m_pModel->Get_CurrAnimation()->Get_TickPerSecond();
	_float fReminingTime = (((65.5f - fCurFrame) / (fTickPerSec * fCurAnimSpeed)));
	_float fSpeed = fDis / fReminingTime;

	m_vFlopPos = vCurPlayerPos;
	m_pTransform->Set_Speed(fSpeed);
}

void CRoland_State_Roll::Belly_Flop(_float fTimeDelta)
{
	_vector vCurPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vTargetPos = m_vFlopPos._vector();

	_float dx = XMVectorGetX(vTargetPos) - XMVectorGetX(vCurPos);
	_float dy = XMVectorGetY(vTargetPos) - XMVectorGetY(vCurPos);
	_float dz = XMVectorGetZ(vTargetPos) - XMVectorGetZ(vCurPos);	

	_float fDis = sqrt(dx * dx + dy * dy + dz * dz);

	if (fDis < 0.1f) {
		m_bIsFlop = false;
		
		Call_Bellyflop_Effect();
		return;
	}

	_float dest = XMVectorGetY(m_pTransform->Get_State(CTransform::STATE::POSITION));
	m_vRollTargetPos.y = dest;
	m_pTransform->LookAt(m_vFlopPos._vector());
	m_pTransform->Go_Straight(fTimeDelta);
}

void CRoland_State_Roll::Roll(_float fTimeDelta){	

	_vec2 vTemp{};
	_vec2 vSour{};

	if (!m_bIsFirstBounce) {
		vTemp = _vec2{ XMVectorGetX(m_pTransform->Get_State(CTransform::STATE::POSITION)), XMVectorGetZ(m_pTransform->Get_State(CTransform::STATE::POSITION)) };
		vSour = _vec2{ m_vRollTargetPos.x, m_vRollTargetPos.z };
	}

	if (Compute_DirectDistance(vTemp, vSour) <= 3.f || m_bIsFirstBounce || m_pRoland->Is_Bounced()) {
		m_vRollTargetPos = m_pRoland->Get_MinAngleTarget();
		_int i = (_uint)CUtils::Compute_Random(1, 10);
		m_pRoland->Set_Bounced(false);

		if (i == 9 && !m_bIsFirstBounce) {
			m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::STUNNED_UNROLL, false, 0.2f);
			m_iCurAnimIndex = (_uint)ROLAND_ANIM_INDEX::STUNNED_UNROLL;
			CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::ENEMY_ATTACK2);
			CGameInstance::GetInstance()->Play_Sound(L"Roland_Unroll.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK2, 0.5f);
			m_pRoland->Disable_RollEffects();
			return;
		}
		else if (i == 8 && !m_bIsFirstBounce) {
			m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.5f);
			Set_BellyFlopSpeed();
			m_bIsFlop = true;
			m_pRoland->Set_Roll(false);			
			m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::UNROLL_INTO_BELLY_FLOP, false, 0.2f);
			m_iCurAnimIndex = (_uint)ROLAND_ANIM_INDEX::UNROLL_INTO_BELLY_FLOP;			
			CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::ENEMY_ATTACK2);
			CGameInstance::GetInstance()->Play_Sound(L"Roland_UnrollIntoBellyFlop.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK2, 0.5f);

			m_pRoland->Disable_RollEffects();
			return;
		}
		m_bIsFirstBounce = false;
	}

	_float temp = XMVectorGetY(m_pTransform->Get_State(CTransform::STATE::POSITION));
	m_vRollTargetPos = XMVectorSetY(m_vRollTargetPos._vector(), temp);
	m_pTransform->LookAt(m_vRollTargetPos._vector());

	_vector vEffectLook = XMVectorSetW(XMVector3Normalize(m_vRollTargetPos._vector() - m_pTransform->Get_State(CTransform::STATE::POSITION)), 0.f);
	m_pRoland->Rotate_RollEffects(vEffectLook);
	m_pRoland->Call_NonLoopRollEffects();

	if(PreRoll(fTimeDelta))
		m_pTransform->Go_Straight(fTimeDelta * 2.f);
}

_bool CRoland_State_Roll::PreRoll(_float fTimeDelta)
{
	if (m_fCurPreRollTime >= m_fMaxPreRollTime)
		return true;		
	else
		m_fCurPreRollTime += fTimeDelta;
	return false;
}

_float CRoland_State_Roll::Compute_DirectDistance(_vec2 vA, _vec2 vB)
{
	_float fX = vA.x - vB.x;
	_float fZ = vA.y - vB.y;
	return sqrt(fX * fX + fZ * fZ);
}

_float CRoland_State_Roll::Compute_Distance(_vec2 vA, _vec2 vB)
{
	_float fX = vA.x - vB.x;
	_float fZ = vA.y - vB.y;
	return sqrt(fX * fX + fZ * fZ);
}

void CRoland_State_Roll::Call_Bellyflop_Effect()
{
	//_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	//_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SLAMSHOCK, vPos, vLook, false, nullptr);
	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SANDBURST, vPos, vLook, false, nullptr);
}

CRoland_State_Roll* CRoland_State_Roll::Create(void* pArg)
{
	CRoland_State_Roll* pInstance = new CRoland_State_Roll();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_Roll");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_Roll::Free()
{
	__super::Free();
}
