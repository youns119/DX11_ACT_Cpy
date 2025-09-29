#include "pch.h"
#include "Cuttlefish_State_ThrowProjectile.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"

#include "Animation.h"


CCuttlefish_State_ThrowProjectile::CCuttlefish_State_ThrowProjectile()
{
}

HRESULT CCuttlefish_State_ThrowProjectile::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Throw Projectile";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 5.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	
	m_pModel = static_cast<CModel*>(static_cast<CEnemy_Cuttlefish*>(pObj)->Find_Component(TEXT("Com_Model")));
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	return S_OK;
}

void CCuttlefish_State_ThrowProjectile::Enter(void* pObj)
{	
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));

	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::PROJECTILE_CAST, false);
	m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::PROJECTILE_CAST;
	__super::Enter(pObj);
}

void CCuttlefish_State_ThrowProjectile::Update(_float fTimeDelta, void* pObj)
{			
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta);

	if (m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::PROJECTILE_CAST && !m_pModel->Get_IsPlaying((_uint)CUTTLEFISH_ANIM_INDEX::PROJECTILE_CAST)) {
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::PROJECTILE_LOOP, false);
		m_iCurAnimIndex = (_uint)CUTTLEFISH_ANIM_INDEX::PROJECTILE_LOOP;
	}
	else if ((m_iCurAnimIndex == (_uint)CUTTLEFISH_ANIM_INDEX::PROJECTILE_LOOP && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		static_cast<CEnemy_Cuttlefish*>(pObj)->Select_Action();
	}

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_ThrowProjectile::Exit(void* pObj)
{
	__super::Exit(pObj);
}

_bool CCuttlefish_State_ThrowProjectile::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.1f, 0.5f);
		m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::IDLE, false, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;
	return false;
}

CCuttlefish_State_ThrowProjectile* CCuttlefish_State_ThrowProjectile::Create(void* pArg)
{
	CCuttlefish_State_ThrowProjectile* pInstance = new CCuttlefish_State_ThrowProjectile();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_ThrowProjectile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_ThrowProjectile::Free()
{
	__super::Free();
}
