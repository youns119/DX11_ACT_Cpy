#include "pch.h"
#include "BobbitWorm_State_Wait.h"
#include "GameInstance.h"
#include "Enemy_BobbitWorm.h"

CBobbitWorm_State_Wait::CBobbitWorm_State_Wait()
{
}

HRESULT CBobbitWorm_State_Wait::Initialize(void* pObj)
{
	m_strStateName = "Wait";

	m_iPriorityLevel = 10;
	m_fMaxCoolTime = 0.f;
	m_pBobbitWorm = static_cast<CEnemy_BobbitWorm*>(pObj);
	m_pTransform = static_cast<CTransform*>(m_pBobbitWorm->Find_Component(TEXT("Com_Transform")));
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CBobbitWorm_State_Wait::Enter(void* pObj)
{
	m_pBobbitWorm->Set_Aggroed(false);
	m_pModel = static_cast<CModel*>(m_pBobbitWorm->Find_Component(TEXT("Com_Model")));
	m_pModel->SetUp_Animation((_uint)BOBBITWORM_ANIM_INDEX::WAITING, true, 0.01f);	

	__super::Enter(pObj);
}

void CBobbitWorm_State_Wait::Update(_float fTimeDelta, void* pObj)
{	
	_vector vCurPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vCurPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	vCurPlayerPos = XMVectorSetY(vCurPlayerPos, XMVectorGetY(vCurPos));
	m_pTransform->LookAt(vCurPlayerPos);

	__super::Update(fTimeDelta, pObj);
}

_float CBobbitWorm_State_Wait::Compute_Distance(_vec2 vA, _vec2 vB)
{
	_float fX = vA.x - vB.x;
	_float fZ = vA.y - vB.y;
	return sqrt(fX * fX + fZ * fZ);
}

void CBobbitWorm_State_Wait::Exit(void* pObj)
{
	__super::Exit(pObj);
}

CBobbitWorm_State_Wait* CBobbitWorm_State_Wait::Create(void* pArg)
{
	CBobbitWorm_State_Wait* pInstance = new CBobbitWorm_State_Wait();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBobbitWorm_State_Wait::Free()
{
	__super::Free();
}
