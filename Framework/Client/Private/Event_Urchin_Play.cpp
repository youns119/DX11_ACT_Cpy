#include "pch.h"
#include "Event_Urchin_Play.h"

#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"

#include "Player.h"

CEvent_Urchin_Play::CEvent_Urchin_Play()
{
}

HRESULT CEvent_Urchin_Play::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pModel = static_cast<CModel*>(_pGameObject->Find_Component(L"Com_Model"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Urchin_Play::Execute()
{
	m_pModel->Get_CurrAnimation()->Set_Active(true);
	m_pPlayer->Get_PlayerStat()->bUrchinThrow = true;

	return S_OK;
}

CEvent_Urchin_Play* CEvent_Urchin_Play::Create(CGameObject* _pGameObject)
{
	CEvent_Urchin_Play* pInstance = new CEvent_Urchin_Play;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Urchin_Play");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Urchin_Play::Free()
{
	super::Free();
}