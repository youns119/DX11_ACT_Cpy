#include "pch.h"
#include "Bowman_State_SnapAttack.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_SnapAttack::CBowman_State_SnapAttack()
{
}

HRESULT CBowman_State_SnapAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Claw Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 5.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Bowman*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Transform")));	

	return S_OK;
}

void CBowman_State_SnapAttack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::HEAVY_SNAP_ATTACK, false, 0.2f);
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	CGameInstance::GetInstance()->Play_Sound(L"Bowman Claw Bash VO Main.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.5f);
	__super::Enter(pObj);
}

void CBowman_State_SnapAttack::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pModel->Get_IsPlaying((_uint)BOWMAN_ANIM_INDEX::HEAVY_SNAP_ATTACK)) {
		static_cast<CEnemy_Bowman*>(pObj)->Select_Action();
	}
	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_SnapAttack::Exit(void* pObj)
{
	__super::Exit(pObj);
}

_bool CBowman_State_SnapAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = (rand() % 20) / 10 + 0.2f;
		m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::IDLE, false, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CBowman_State_SnapAttack* CBowman_State_SnapAttack::Create(void* pArg)
{
	CBowman_State_SnapAttack* pInstance = new CBowman_State_SnapAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_SnapAttack::Free()
{
	__super::Free();
}