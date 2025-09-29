#include "pch.h"
#include "Event_Urchin_Stop.h"

#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

#include "Player.h"

CEvent_Urchin_Stop::CEvent_Urchin_Stop()
{
}

HRESULT CEvent_Urchin_Stop::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pModel = static_cast<CModel*>(_pGameObject->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Urchin_Stop::Execute()
{
	if (m_pPlayer->Get_PlayerStat()->bUrchinThrow)
		m_pModel->Get_CurrAnimation()->Set_Active(true);
	else
	{
		m_pModel->Get_CurrAnimation()->Set_Active(false);
		m_pMovement->Activate(false);
		m_pMovement->Reset_Velocity();
	}

	return S_OK;
}

CEvent_Urchin_Stop* CEvent_Urchin_Stop::Create(CGameObject* _pGameObject)
{
	CEvent_Urchin_Stop* pInstance = new CEvent_Urchin_Stop;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Urchin_Stop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Urchin_Stop::Free()
{
	super::Free();
}