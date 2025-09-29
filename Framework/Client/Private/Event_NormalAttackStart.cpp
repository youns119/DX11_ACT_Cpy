#include "pch.h"
#include "Event_NormalAttackStart.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Movement.h"

#include "Player.h"

CEvent_NormalAttackStart::CEvent_NormalAttackStart()
{
}

HRESULT CEvent_NormalAttackStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);
	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_NormalAttackStart::Execute()
{
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	m_pMovement->Add_Force(vLook, 30.f);

	m_pPlayerStat->fAnimSpeed = 1.f;

	switch (m_pPlayerStat->iComboCount)
	{
	case 1:
		m_pPlayer->Play_PlayerSound(L"NormalAttack1");
		break;
	case 2:
		m_pPlayer->Play_PlayerSound(L"NormalAttack2");
		break;
	case 3:
		m_pPlayer->Play_PlayerSound(L"NormalAttack3");
		break;
	case 4:
		m_pPlayer->Play_PlayerSound(L"NormalAttack4");
		m_pPlayer->Play_PlayerSound(L"NormalAttack_Voice");
		break;
	default:
		break;
	}

	if (CGameInstance::GetInstance()->Get_CurrentLevelIndex() != LEVEL_BEACH)
		m_pPlayer->Play_PlayerSound(L"NormalAttack_Bubble");

	return S_OK;
}

CEvent_NormalAttackStart* CEvent_NormalAttackStart::Create(CGameObject* _pGameObject)
{
	CEvent_NormalAttackStart* pInstance = new CEvent_NormalAttackStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_NormalAttackStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_NormalAttackStart::Free()
{
	super::Free();
}