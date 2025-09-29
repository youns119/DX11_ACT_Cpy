#include "pch.h"
#include "Cuttlefish_State_Execution.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"

CCuttlefish_State_Execution::CCuttlefish_State_Execution()
{
}

HRESULT CCuttlefish_State_Execution::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Execution";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 40.f;

	m_pCuttlefish = static_cast<CEnemy_Cuttlefish*>(pObj);
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));		
	m_pMovement = static_cast<CMovement*>(m_pCuttlefish->Find_Component(TEXT("Com_Movement")));
	m_pModel = static_cast<CModel*>(m_pCuttlefish->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CCuttlefish_State_Execution::Enter(void* pObj)
{			
	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::HYPNOSIS_ENTER, false);
	m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::HYPNOSIS_ENTER;

	m_fMaxHypnosisDelay = CUtils::Compute_Random(0.f, 1.f);

	CGameInstance::GetInstance()->Play_Sound(L"Cuttle_HypnoEffort1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);

	__super::Enter(pObj);
}

void CCuttlefish_State_Execution::Update(_float fTimeDelta, void* pObj)
{			
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta);
	
	m_fCurDensity += fTimeDelta;
	m_fCurIntensity += fTimeDelta;

	if (m_fCurIntensity >= m_fTargetIntensity) {
		m_fCurIntensity = m_fTargetIntensity;
	}
	if (m_fCurDensity >= m_fTargetDensity) {
		m_fCurDensity = m_fTargetDensity;
	}

	RADIALBLUR_PARAMS tRadialParam{};
	tRadialParam.fDensity = m_fCurIntensity;
	tRadialParam.fIntensity = m_fCurDensity;
	CGameInstance::GetInstance()->Set_RadialBlurParams(tRadialParam);

	if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::HYPNOSIS_ENTER && !m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::HYPNOSIS_ENTER)) {
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::HYPNOSIS_LOOP, false);
		m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::HYPNOSIS_LOOP;
	}
	if (Hypnosis_Delay(fTimeDelta)) {
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::HYPNOTIZE_GRAB_ATTEMPT, false);
		m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::HYPNOTIZE_GRAB_ATTEMPT;
		m_fCurActionDelay = 0;
		m_pCuttlefish->Set_IsGrab(true);

		CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Grab.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK, 0.7f);
		CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Grab_VO2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.7f);		
	}
	if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::HYPNOTIZE_GRAB_ATTEMPT && !m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::HYPNOTIZE_GRAB_ATTEMPT)) {		
		m_pCuttlefish->Set_IsGrab(false);
		if (m_pCuttlefish->Is_Execution()) {
			m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::HYPNOTIZE_GRAB_SUCCESS, false);
			m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::HYPNOTIZE_GRAB_SUCCESS;			
		}
		else {
			Reset_RadialblurParams();
			m_pCuttlefish->Select_Action();
		}
	}
	if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::HYPNOTIZE_GRAB_SUCCESS && !m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::HYPNOTIZE_GRAB_SUCCESS)) {
		if (After_AttackDelay(fTimeDelta)) {
			Reset_RadialblurParams();
			m_pCuttlefish->Select_Action();
		}
	}

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Execution::Exit(void* pObj)
{
	m_pCuttlefish->Set_Execution(false);
	m_fCurActionDelay = 0;
	__super::Exit(pObj);
}

void CCuttlefish_State_Execution::Reset_RadialblurParams()
{
	RADIALBLUR_PARAMS tRadialParam{};
	tRadialParam.fDensity = 2;
	tRadialParam.fIntensity = 0;
	CGameInstance::GetInstance()->Set_RadialBlurParams(tRadialParam);
}

_bool CCuttlefish_State_Execution::Hypnosis_Delay(_float fTimeDelta)
{
	if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::HYPNOSIS_LOOP && m_fCurActionDelay >= m_fMaxHypnosisDelay)
		return true;		
	else
		m_fCurActionDelay += fTimeDelta;
	return false;
}

_bool CCuttlefish_State_Execution::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 0.25f);
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::IDLE, false, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;
	return false;
}

CCuttlefish_State_Execution* CCuttlefish_State_Execution::Create(void* pArg)
{
	CCuttlefish_State_Execution* pInstance = new CCuttlefish_State_Execution();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Execution");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_Execution::Free()
{
	__super::Free();
}

