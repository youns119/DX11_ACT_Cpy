#include "pch.h"
#include "Bowman_State_Backflip.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_Backflip::CBowman_State_Backflip()
{
}

HRESULT CBowman_State_Backflip::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "BackFlip";
#endif

	m_iPriorityLevel = 2;
	m_fMaxCoolTime = 20.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Bowman*>(pObj)->Find_Component(TEXT("Com_Model")));	

	return S_OK;
}

void CBowman_State_Backflip::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::JUMP_BACKWARDS, false, 0.2f);

	CGameInstance::GetInstance()->Play_Sound(L"Bowman_Retreat.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.5f);	
	__super::Enter(pObj);
}  

void CBowman_State_Backflip::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)BOWMAN_ANIM_INDEX::JUMP_BACKWARDS)) {
		static_cast<CEnemy_Bowman*>(pObj)->Set_IsChase(false);
		static_cast<CEnemy_Bowman*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_Backflip::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CBowman_State_Backflip* CBowman_State_Backflip::Create(void* pArg)
{
	CBowman_State_Backflip* pInstance = new CBowman_State_Backflip();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CBowman_State_Backflip");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_Backflip::Free()
{
	__super::Free();
}
