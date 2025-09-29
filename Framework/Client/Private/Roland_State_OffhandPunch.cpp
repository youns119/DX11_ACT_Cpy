#include "pch.h"
#include "Roland_State_OffhandPunch.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "Animation.h"

#include "Roland_Fist.h"

CRoland_State_OffhandPunch::CRoland_State_OffhandPunch()
{
}

HRESULT CRoland_State_OffhandPunch::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Offhand Punch";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CRoland_State_OffhandPunch::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)ROLAND_ANIM_INDEX::OFFHAND_PUNCH, false, 0.2f);

	super::Enter(pObj);
}

void CRoland_State_OffhandPunch::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)ROLAND_ANIM_INDEX::OFFHAND_PUNCH) || m_iAnimIndex == 0 && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		CBoss_Roland* pRoland = static_cast<CBoss_Roland*>(pObj);
		if (!static_cast<CRoland_Fist*>(pRoland->Find_PartObject({ L"Part_Fist" }))->Is_Hit())
			CGameInstance::GetInstance()->Play_Sound(L"Roland_PunchWhiff.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);
		static_cast<CBoss_Roland*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CRoland_State_OffhandPunch::Exit(void* pObj)
{	
	CBoss_Roland* pRoland = static_cast<CBoss_Roland*>(pObj);
	static_cast<CRoland_Fist*>(pRoland->Find_PartObject({ L"Part_Fist" }))->Set_Hit(false);
	super::Exit(pObj);
}

CRoland_State_OffhandPunch* CRoland_State_OffhandPunch::Create(void* pArg)
{
	CRoland_State_OffhandPunch* pInstance = new CRoland_State_OffhandPunch();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CRoland_State_OffhandPunch");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CRoland_State_OffhandPunch::Free()
{
	__super::Free();
}
