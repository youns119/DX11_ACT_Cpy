#include "pch.h"
#include "Normie_State_Walk.h"
#include "GameInstance.h"
#include "Enemy_Normie.h"


CNormie_State_Walk::CNormie_State_Walk()
{
}

HRESULT CNormie_State_Walk::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;
	
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Normie*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));	

	return S_OK;
}

void CNormie_State_Walk::Enter(void* pObj)
{	
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	m_pModel->SetUp_Animation((_uint)NORMIE_ANIM_INDEX::IDLESAND , false);
	//CGameInstance::GetInstance()->Play_Sound(L"Sardine Aggro 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CNormie_State_Walk::Update(_float fTimeDelta, void* pObj)
{		
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta * 0.5f);
	m_pTransform->Go_Straight(fTimeDelta * 0.6f);

	_vector vDis = vLookTarget - m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = XMVectorGetX(XMVector3Length(vDis));

	if (fDis < 6) {
		static_cast<CEnemy_Normie*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CNormie_State_Walk::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CNormie_State_Walk* CNormie_State_Walk::Create(void* pArg)
{
	CNormie_State_Walk* pInstance = new CNormie_State_Walk();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNormie_State_Walk::Free()
{
	__super::Free();	
}