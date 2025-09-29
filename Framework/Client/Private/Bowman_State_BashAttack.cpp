#include "pch.h"
#include "Bowman_State_BashAttack.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_BashAttack::CBowman_State_BashAttack()
{
}

HRESULT CBowman_State_BashAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Bash Attack";
#endif

	m_iPriorityLevel = 5;
	m_fMaxCoolTime = 10.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Bowman*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	return S_OK;
}

void CBowman_State_BashAttack::Enter(void* pObj)
{	
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::CLAW_BASH, false);
	__super::Enter(pObj);
}

void CBowman_State_BashAttack::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)BOWMAN_ANIM_INDEX::CLAW_BASH)) {
		if (After_AttackDelay(fTimeDelta)) {
			static_cast<CEnemy_Bowman*>(pObj)->Select_Action();
		}
	}

	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_BashAttack::Exit(void* pObj)
{
	__super::Exit(pObj);
}

_bool CBowman_State_BashAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = (rand() % 20) / 10 + 0.5f;
		m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::IDLE, false, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;
	return false;
}

CBowman_State_BashAttack* CBowman_State_BashAttack::Create(void* pArg)
{
	CBowman_State_BashAttack* pInstance = new CBowman_State_BashAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CBowman_State_BashAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_BashAttack::Free()
{
	__super::Free();
}
