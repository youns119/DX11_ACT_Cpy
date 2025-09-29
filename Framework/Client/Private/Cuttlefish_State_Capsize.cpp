#include "pch.h"
#include "Cuttlefish_State_Capsize.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_Capsize::CCuttlefish_State_Capsize()
{
}

HRESULT CCuttlefish_State_Capsize::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Capsized";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 10.f;	

	m_pCuttlefish = static_cast<CEnemy_Cuttlefish*>(pObj);
	m_pModel = static_cast<CModel*>(m_pCuttlefish->Find_Component(TEXT("Com_Model")));

	m_Sounds.push_back(L"Cuttle_Prone1.wav");
	m_Sounds.push_back(L"Cuttle_Prone2.wav");
	m_Sounds.push_back(L"Cuttle_Prone3.wav");	

	return S_OK;
}

void CCuttlefish_State_Capsize::Enter(void* pObj)
{		
	Reset_RadialblurParams();

	CEnemy_Cuttlefish* pCuttlefish = static_cast<CEnemy_Cuttlefish*>(pObj);
	static_cast<CCollider*>(pCuttlefish->Find_PartObject({ TEXT("Part_Fist") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pCuttlefish->Find_PartObject({ TEXT("Part_Tail") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pCuttlefish->Find_PartObject({ TEXT("Part_Grab") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	pCuttlefish->Set_IsChase(false);
	pCuttlefish->Set_IsSideMove(false);

	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_ENTER, false);
	m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_ENTER;

	_int ISoundIndex = CUtils::Compute_Random(0, m_Sounds.size() - 1);
	CGameInstance::GetInstance()->Play_Sound(m_Sounds[ISoundIndex], (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CCuttlefish_State_Capsize::Update(_float fTimeDelta, void* pObj)
{
	if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_ENTER && !m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_ENTER)) {
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_LOOP, true, 0.2f);
		m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_LOOP;
	}
	else if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_LOOP && Decrease_CapsizedDuration(fTimeDelta) && !m_pCuttlefish->IsFinisher()) {
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_EXIT, false, 0.2f);
		m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_EXIT;
	}
	else if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_EXIT && !m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::FLIPPED_EXIT)) {
		m_pCuttlefish->Set_Capsized(false);
		m_pCuttlefish->Reset_Balance();
		m_pCuttlefish->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Capsize::Exit(void* pObj)
{
	m_pCuttlefish->Set_Finished(false);
	m_fCapsizeTime = 0;

	__super::Exit(pObj);
}


void CCuttlefish_State_Capsize::Reset_RadialblurParams()
{
	RADIALBLUR_PARAMS tRadialParam{};
	tRadialParam.fDensity = 2;
	tRadialParam.fIntensity = 0;
	CGameInstance::GetInstance()->Set_RadialBlurParams(tRadialParam);
}

_bool CCuttlefish_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}

CCuttlefish_State_Capsize* CCuttlefish_State_Capsize::Create(void* pArg)
{
	CCuttlefish_State_Capsize* pInstance = new CCuttlefish_State_Capsize();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Capsize");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_Capsize::Free()
{
	__super::Free();
}
