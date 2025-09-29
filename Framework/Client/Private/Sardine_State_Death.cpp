#include "pch.h"
#include "Sardine_State_Death.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"

#include "Animation.h"


CSardine_State_Death::CSardine_State_Death()
{
}

HRESULT CSardine_State_Death::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Death";
#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Model")));			

	return S_OK;
}

void CSardine_State_Death::Enter(void* pObj)
{	
	CEnemy_Sardine* pSardine = static_cast<CEnemy_Sardine*>(pObj);
	static_cast<CCollider*>(pSardine->Find_PartObject({ TEXT("Part_Horn") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pSardine->Find_PartObject({ TEXT("Part_Tail") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pSardine->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	pSardine->Set_IsChase(false);
	pSardine->Drop_Loot();
	pSardine->Death();
	
	m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(0.f);

	CGameInstance::GetInstance()->Play_Sound(L"Sardine Death 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, .7f);

	__super::Enter(pObj);
}

void CSardine_State_Death::Update(_float fTimeDelta, void* pObj)
{
	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_Death::Exit(void* pObj)
{
	m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.f);

	__super::Exit(pObj);
}

CSardine_State_Death* CSardine_State_Death::Create(void* pArg)
{
	CSardine_State_Death* pInstance = new CSardine_State_Death();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_Death::Free()
{
	__super::Free();
}
