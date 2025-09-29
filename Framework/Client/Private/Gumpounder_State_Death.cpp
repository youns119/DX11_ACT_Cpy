#include "pch.h"
#include "Gumpounder_State_Death.h"
#include "GameInstance.h"
#include "Enemy_Gumpounder.h"


CGumpounder_State_Death::CGumpounder_State_Death()
{
}

HRESULT CGumpounder_State_Death::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Aggroed";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Gumpounder*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CGumpounder_State_Death::Enter(void* pObj)
{	
	CEnemy_Gumpounder* pNormie = static_cast<CEnemy_Gumpounder*>(pObj);
	static_cast<CCollider*>(pNormie->Find_PartObject({ TEXT("Part_LeftClaw") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);	
	static_cast<CCollider*>(pNormie->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	pNormie->Set_IsChase(false);
	m_pModel->SetUp_Animation((_uint)GUMPOUNDER_ANIM_INDEX::DIE, false);

	//CGameInstance::GetInstance()->Play_Sound(L"Sardine Aggro 1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CGumpounder_State_Death::Update(_float fTimeDelta, void* pObj)
{
	__super::Update(fTimeDelta, pObj);
}

void CGumpounder_State_Death::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CGumpounder_State_Death* CGumpounder_State_Death::Create(void* pArg)
{
	CGumpounder_State_Death* pInstance = new CGumpounder_State_Death();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGumpounder_State_Death::Free()
{
	__super::Free();	
}