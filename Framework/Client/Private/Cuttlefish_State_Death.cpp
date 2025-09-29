#include "pch.h"
#include "Cuttlefish_State_Death.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_Death::CCuttlefish_State_Death()
{
}

HRESULT CCuttlefish_State_Death::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "daeth";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 10.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Cuttlefish*>(pObj)->Find_Component(TEXT("Com_Model")));	

	m_Sounds.push_back(L"Cuttle_Dead1.wav");
	m_Sounds.push_back(L"Cuttle_Dead2.wav");
	m_Sounds.push_back(L"Cuttle_Dead3.wav");
	m_Sounds.push_back(L"Cuttle_Dead4.wav");
	m_Sounds.push_back(L"Cuttle_Dead5.wav");

	return S_OK;
}

void CCuttlefish_State_Death::Enter(void* pObj)
{		
	CEnemy_Cuttlefish* pCuttlefish = static_cast<CEnemy_Cuttlefish*>(pObj);
	static_cast<CCollider*>(pCuttlefish->Find_PartObject({ TEXT("Part_Fist") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pCuttlefish->Find_PartObject({ TEXT("Part_Tail") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pCuttlefish->Find_PartObject({ TEXT("Part_Grab") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pCuttlefish->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	pCuttlefish->Set_IsChase(false);
	pCuttlefish->Set_IsSideMove(false);

	pCuttlefish->Drop_Loot();
	pCuttlefish->Death();

	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::DIE, false);

	_int ISoundIndex = CUtils::Compute_Random(0, m_Sounds.size() - 1);
	CGameInstance::GetInstance()->Play_Sound(m_Sounds[ISoundIndex], (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);

	__super::Enter(pObj);
}

void CCuttlefish_State_Death::Update(_float fTimeDelta, void* pObj)
{		
	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Death::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CCuttlefish_State_Death* CCuttlefish_State_Death::Create(void* pArg)
{
	CCuttlefish_State_Death* pInstance = new CCuttlefish_State_Death();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Death");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_Death::Free()
{
	__super::Free();
}
