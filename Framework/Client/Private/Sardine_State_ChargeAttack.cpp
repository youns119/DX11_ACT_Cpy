#include "pch.h"
#include "Sardine_State_ChargeAttack.h"
#include "GameInstance.h"
#include "Enemy_Sardine.h"


CSardine_State_ChargeAttack::CSardine_State_ChargeAttack()
{
}

HRESULT CSardine_State_ChargeAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Charge Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 3.f;

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Sardine*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CSardine_State_ChargeAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::TAIL_ATTACK, false);

	m_fMaxActionDelay = CUtils::Compute_Random(0.3f, 1.f);
	CGameInstance::GetInstance()->Play_Sound(L"Sardine Attack Swoosh 1.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE1, 0.5f);
	__super::Enter(pObj);
}

void CSardine_State_ChargeAttack::Update(_float fTimeDelta, void* pObj)
{				
	if (!m_pModel->Get_IsPlaying((_uint)SARDINE_ANIM_INDEX::TAIL_ATTACK)) {
		if(After_AttackDelay(fTimeDelta))
			static_cast<CEnemy_Sardine*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CSardine_State_ChargeAttack::Exit(void* pObj)
{
	Set_MaxCooldownTime(CUtils::Compute_Random(1.5f, 3.f));
	m_bIsAnimChanged = false;
	m_fCurActionDelay = 0;
	__super::Exit(pObj);
}

_bool CSardine_State_ChargeAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = (rand() % 20) / 10 + 0.5f;
		m_pModel->SetUp_Animation((_uint)SARDINE_ANIM_INDEX::SWIM_FORWARD, true, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CSardine_State_ChargeAttack* CSardine_State_ChargeAttack::Create(void* pArg)
{
	CSardine_State_ChargeAttack* pInstance = new CSardine_State_ChargeAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSardine_State_ChargeAttack::Free()
{
	__super::Free();	
}