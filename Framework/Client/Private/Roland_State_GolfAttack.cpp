#include "pch.h"
#include "Roland_State_GolfAttack.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"

#include "Roland_Wrench_0_R.h"

CRoland_State_GolfAttack::CRoland_State_GolfAttack()
{
}

HRESULT CRoland_State_GolfAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Golf Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_GolfAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::GOLF_WINDUP, false, 0.2f);

	CBoss_Roland* pRoland = static_cast<CBoss_Roland*>(pObj);
	static_cast<CRoland_Wrench_0_R*>(pRoland->Find_PartObject({ L"Part_Wrench_0_R" }))->Set_GolfAttack(true);

	super::Enter(pObj);
}

void CRoland_State_GolfAttack::Update(_float fTimeDelta, void* pObj)
{					
	if (m_iAnimIndex == 0 && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::GOLF_WINDUP)) {
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::GOLF_BACK_HOP, false, 0.2f);
		m_iAnimIndex++;

		CGameInstance::GetInstance()->Play_Sound(L"Roland GolfTeeUp.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 1.f);
	}
	else if (m_iAnimIndex == 1 && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::GOLF_BACK_HOP)) {
		m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::GOLF_ATTACK, false, 0.2f);
		m_iAnimIndex++;
		CGameInstance::GetInstance()->Play_Sound(L"Block1 - 2.wav.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 1.f);
	}
	else if (m_iAnimIndex == 2 && !m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::GOLF_ATTACK)) {	
		CBoss_Roland* pRoland = static_cast<CBoss_Roland*>(pObj);
		static_cast<CRoland_Wrench_0_R*>(pRoland->Find_PartObject({ L"Part_Wrench_0_R" }))->Set_GolfAttack(false);
		//CGameInstance::GetInstance()->Play_Sound(L"1Roland GolfSwing.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 1.f);
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_GolfAttack::Exit(void* pObj)
{	
	m_iAnimIndex = 0;
	super::Exit(pObj);
}

CRoland_State_GolfAttack* CRoland_State_GolfAttack::Create(void* pArg)
{
	CRoland_State_GolfAttack* pInstance = new CRoland_State_GolfAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_GolfAttack");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_GolfAttack::Free()
{
	__super::Free();
}
