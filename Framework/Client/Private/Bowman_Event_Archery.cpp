#include "pch.h"
#include "Bowman_Event_Archery.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"
#include "Bowman_Arrow.h"


CBowman_Event_Archery::CBowman_Event_Archery():
	m_pGameInstance(CGameInstance::GetInstance())
{
	m_pGameInstance->AddRef();
}


HRESULT CBowman_Event_Archery::Execute()
{	
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();	
	_vector m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE::POSITION);

	static_cast<CBowman_Arrow*>(static_cast<CEnemy_Bowman*>(m_pGameObject)->Find_PartObject({ TEXT("Part_Arrow") }))->Trigger_Arrow(m_pPlayerTransform);
	return S_OK;
}

CBowman_Event_Archery* CBowman_Event_Archery::Create(CGameObject* _pGameObject)
{
	CBowman_Event_Archery* pInstance = new CBowman_Event_Archery();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CBowman_Event_Archery");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowman_Event_Archery::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
