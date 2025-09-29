#include "pch.h"
#include "Bowman_State_RangeAttack.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"


CBowman_State_RangeAttack::CBowman_State_RangeAttack()
{
}

HRESULT CBowman_State_RangeAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Range Attack";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 2.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	CEnemy_Bowman* pBowman = static_cast<CEnemy_Bowman*>(pObj);
	m_pModel = static_cast<CModel*>(pBowman->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(pBowman->Find_Component(TEXT("Com_Transform")));	

	_uint iLArmIndex = m_pModel->Get_BoneIndex_By_Name("Shoulder_L.18");
	_uint iRArmIndex = m_pModel->Get_BoneIndex_By_Name("Shoulder_R.29");

	m_pLArmTransform = m_pModel->Get_BoneSperateTransformCom(iLArmIndex);
	m_pRArmTransform = m_pModel->Get_BoneSperateTransformCom(iRArmIndex);

	return S_OK;
}

void CBowman_State_RangeAttack::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)BOWMAN_ANIM_INDEX::FIRE_ARROW, false, 0.5f);
	m_pMovement = static_cast<CMovement*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Movement")));
	__super::Enter(pObj);
}

void CBowman_State_RangeAttack::Update(_float fTimeDelta, void* pObj)
{	
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta * 0.8f);

	_float3 BasePos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
	_float3 TargetPos = { XMVectorGetX(vLookTarget), XMVectorGetY(vLookTarget), XMVectorGetZ(vLookTarget) };

	_float fDiff = TargetPos.y - BasePos.y;

	_float fDistance = sqrtf((BasePos.x - TargetPos.x) * (BasePos.x - TargetPos.x) + (BasePos.z - TargetPos.z) * (BasePos.z - TargetPos.z));
	_float fRadians = fDiff / fDistance;

	m_pLArmTransform->Rotate(m_pLArmTransform->Get_State(CTransform::STATE::RIGHT), fRadians);
	m_pRArmTransform->Rotate(m_pRArmTransform->Get_State(CTransform::STATE::RIGHT), fRadians);

	if (!m_pModel->Get_IsPlaying((_uint)BOWMAN_ANIM_INDEX::FIRE_ARROW)) {
		if (After_AttackDelay(fTimeDelta)) {
			static_cast<CEnemy_Bowman*>(pObj)->Select_Action();
		}
	}
	__super::Update(fTimeDelta, pObj);
}

void CBowman_State_RangeAttack::Exit(void* pObj)
{	
	__super::Exit(pObj);
}

_bool CBowman_State_RangeAttack::After_AttackDelay(_float fTimeDelta)
{
	if (!m_bIsAnimChanged) {
		m_fMaxActionDelay = CUtils::Compute_Random(0.3f, 1.5f);;
		m_pModel->SetUp_Animation(9, false, 0.3f);
		m_bIsAnimChanged = true;
	}
	if (m_fCurActionDelay >= m_fMaxActionDelay)
		return true;
	else
		m_fCurActionDelay += fTimeDelta;

	return false;
}

CBowman_State_RangeAttack* CBowman_State_RangeAttack::Create(void* pArg)
{
	CBowman_State_RangeAttack* pInstance = new CBowman_State_RangeAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSplitter_State_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowman_State_RangeAttack::Free()
{
	__super::Free();
}
