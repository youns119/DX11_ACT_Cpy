#include "pch.h"
#include "Inkerton_State_GunShot.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"


CInkerton_State_GunShot::CInkerton_State_GunShot()
{
}

HRESULT CInkerton_State_GunShot::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Gun Shot";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;	

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}


void CInkerton_State_GunShot::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::AIM, false, 0.2f);
	m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::AIM;
	m_fCurActionDelay = 0;	

	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Aim_Exciter.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK2, 1.f);
	super::Enter(pObj);
}

void CInkerton_State_GunShot::Update(_float fTimeDelta, void* pObj)
{			
	if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::AIM && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::AIM)) {
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::FIRE, false, 0.2f);
		m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::FIRE;
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::FIRE && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::FIRE)) {
		if(After_AttackDelay(fTimeDelta))
			static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_GunShot::Exit(void* pObj)
{	
	super::Exit(pObj);
}

_bool CInkerton_State_GunShot::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.5f, 1.f);;
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}


CInkerton_State_GunShot* CInkerton_State_GunShot::Create(void* pArg)
{
	CInkerton_State_GunShot* pInstance = new CInkerton_State_GunShot();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_GunShot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_GunShot::Free()
{
	__super::Free();
}
