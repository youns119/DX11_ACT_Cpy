#include "pch.h"
#include "Cuttlefish_State_Move.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"
#include "Animation.h"


CCuttlefish_State_Move::CCuttlefish_State_Move()
{
}

HRESULT CCuttlefish_State_Move::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Move";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 1.f;

	m_pCuttlefish = static_cast<CEnemy_Cuttlefish*>(pObj);
	m_pTransform = static_cast<CTransform*>(m_pCuttlefish->Find_Component(TEXT("Com_Transform")));
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CCuttlefish_State_Move::Enter(void* pObj)
{
	_uint iRanNum = (_uint)CUtils::Compute_Random(1.f, 10.f);

	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::IDLE, true, 0.2f);
	m_iAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::IDLE;
			
	m_fMoveDuration = CUtils::Compute_Random(1.f, 5.f);

	CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Wooh1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);

	super::Enter(pObj);
}

void CCuttlefish_State_Move::Update(_float fTimeDelta, void* pObj)
{
	Decrease_Duration(fTimeDelta);

	super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Move::Exit(void* pObj)
{	
	m_fCurMoveDuration = 0;

	m_pCuttlefish->Set_IsChase(false);
	m_pCuttlefish->Set_IsSideMove(false);

	super::Exit(pObj);
}

void CCuttlefish_State_Move::Decrease_Duration(_float fTimeDelta)
{
	if (m_fCurMoveDuration >= m_fMoveDuration)
		return;

	m_fCurActDuration += fTimeDelta;
	m_fCurMoveDuration += fTimeDelta;

	if (m_fCurMoveDuration >= m_fMoveDuration) {
		m_pCuttlefish->Select_Action();
	}
	else if (m_fCurActDuration >= m_fActDuration) {
		Select_Action();
		m_fCurActDuration = 0;
	}
}

void CCuttlefish_State_Move::Select_Action()
{
	_int iRand = CUtils::Compute_Random(0, 2);
	switch (iRand)
	{
	case 0:
		m_pCuttlefish->Set_IsChase(true, -0.5f);
		m_pCuttlefish->Set_IsSideMove(false);
		break;
	case 1:
		m_pCuttlefish->Set_IsChase(false);
		m_pCuttlefish->Set_IsSideMove(true, 0.5f);
		break;
	default:
		_vector vTemp = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vSour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
		_float fDest = XMVectorGetX(XMVector4Length(vTemp - vSour));
		if (fDest > 4.f) {
			m_pCuttlefish->Set_IsChase(true, 0.5f);
			m_pCuttlefish->Set_IsSideMove(false);
		}
		else {
			m_pCuttlefish->Set_IsChase(true, -0.5f);
			m_pCuttlefish->Set_IsSideMove(false);
		}
		break;
	}

	m_fActDuration = CUtils::Compute_Random(1.f, 4.f);
}

CCuttlefish_State_Move* CCuttlefish_State_Move::Create(void* pArg)
{
	CCuttlefish_State_Move* pInstance = new CCuttlefish_State_Move();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Move");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CCuttlefish_State_Move::Free()
{
	__super::Free();
}
