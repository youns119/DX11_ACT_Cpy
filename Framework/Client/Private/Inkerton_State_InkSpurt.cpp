#include "pch.h"
#include "Inkerton_State_InkSpurt.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"


CInkerton_State_InkSpurt::CInkerton_State_InkSpurt()
{
}

HRESULT CInkerton_State_InkSpurt::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Light Charge Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 20.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));
	//Inkerton_Attack_4_Exciter
	return S_OK;
}


void CInkerton_State_InkSpurt::Enter(void* pObj)
{		
	_vector vPos = static_cast<CTransform*>(CGameInstance::GetInstance()->Find_Component(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player"), g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ENEMY_SPRITE_SQUID, vPos, vLook, false, nullptr);

	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::PEACE_OUT, false, 0.2f);

	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Attack_4_Exciter.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, .5f);
	super::Enter(pObj);
}

void CInkerton_State_InkSpurt::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::PEACE_OUT) || (m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		//m_iAnimIndex = 1;
		//After_AttackDelay(fTimeDelta);
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_InkSpurt::Exit(void* pObj)
{	
	m_fChaseSpeed = 0.5f;
	m_iAnimIndex = 0;
	super::Exit(pObj);	
}

_bool CInkerton_State_InkSpurt::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 1.f);
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::GRAB_ATTEMPT, true, 0.3f);
		
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CInkerton_State_InkSpurt* CInkerton_State_InkSpurt::Create(void* pArg)
{
	CInkerton_State_InkSpurt* pInstance = new CInkerton_State_InkSpurt();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_InkSpurt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_InkSpurt::Free()
{
	__super::Free();
}
