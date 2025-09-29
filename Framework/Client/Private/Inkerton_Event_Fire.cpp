#include "pch.h"
#include "Inkerton_Event_Fire.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"
#include "Inkerton_Gun.h"

CInkerton_Event_Fire::CInkerton_Event_Fire()	
{
}

HRESULT CInkerton_Event_Fire::Execute()
{		

	static_cast<CInkerton_Gun*>(static_cast<CBoss_Inkerton*>(m_pGameObject)->Find_PartObject({ L"Part_Gun" }))->Fire_Bullet();
	return S_OK;
}

CInkerton_Event_Fire* CInkerton_Event_Fire::Create(CGameObject* _pGameObject)
{
	CInkerton_Event_Fire* pInstance = new CInkerton_Event_Fire();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Fire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Fire::Free()
{
	__super::Free();
}
