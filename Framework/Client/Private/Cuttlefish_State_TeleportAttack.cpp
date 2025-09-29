#include "pch.h"
#include "Cuttlefish_State_TeleportAttack.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_TeleportAttack::CCuttlefish_State_TeleportAttack()
{
}

HRESULT CCuttlefish_State_TeleportAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Teleport Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 5.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Cuttlefish*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));
	m_pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pObj)->Find_Component(g_strTransformTag));
	return S_OK;
}

void CCuttlefish_State_TeleportAttack::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::INK_TELEPORT_ENTER, false);
	m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::INK_TELEPORT_ENTER;
	CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Wooh6", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CCuttlefish_State_TeleportAttack::Update(_float fTimeDelta, void* pObj)
{			
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta);

	if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::INK_TELEPORT_ENTER && !m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::INK_TELEPORT_ENTER)) {
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::INK_TELEPORT_EXIT, false);
		m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::INK_TELEPORT_EXIT;

		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_float fDistance =  XMVectorGetX(XMVector4Length(vPosition - vLookTarget)) - 3.5f;
		_vector vTarget = XMVectorAdd(vLookTarget, XMVectorScale(vLook, fDistance));
		vTarget = XMVectorSetY(vTarget, XMVectorGetY(vTarget) + 1.f);
		static_cast<CEnemy_Cuttlefish*>(pObj)->Locate(vTarget);
	}
	if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::INK_TELEPORT_EXIT && !m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::INK_TELEPORT_EXIT)) {
		if (After_AttackDelay(fTimeDelta)) {
			static_cast<CEnemy_Cuttlefish*>(pObj)->Select_Action();
		}
	}

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_TeleportAttack::Exit(void* pObj)
{
	__super::Exit(pObj);
}

_bool CCuttlefish_State_TeleportAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 0.2f);
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::IDLE, false, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;
	return false;
}

CCuttlefish_State_TeleportAttack* CCuttlefish_State_TeleportAttack::Create(void* pArg)
{
	CCuttlefish_State_TeleportAttack* pInstance = new CCuttlefish_State_TeleportAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_TeleportAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_TeleportAttack::Free()
{
	__super::Free();
}
