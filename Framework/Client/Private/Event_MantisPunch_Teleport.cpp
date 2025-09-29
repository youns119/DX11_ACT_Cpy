#include "pch.h"
#include "Event_MantisPunch_Teleport.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Movement.h"

CEvent_MantisPunch_Teleport::CEvent_MantisPunch_Teleport()
{
}

HRESULT CEvent_MantisPunch_Teleport::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	return S_OK;
}

HRESULT CEvent_MantisPunch_Teleport::Execute()
{
	m_pPlayer->Set_AutoLock(false);

	_vec3 vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	_vec3 vTargetPos{};
	CGameObject* pTarget = m_pPlayer->Get_AutoLockTarget();

	if (pTarget == nullptr)
		vTargetPos = vPos + vLook * 20.f;
	else
	{
		CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Find_Component(g_strTransformTag));
		vTargetPos = pTargetTransform->Get_State(CTransform::STATE::POSITION);

		vTargetPos = vTargetPos - vLook * 5.f;
	}

	_vec3 vDir = (vTargetPos - vPos).Normalized();
	m_pTransform->LookAt((vPos + vDir)._vector());

	m_pPlayer->Move_Player(vTargetPos._vector());

	m_pPlayer->Get_PlayerCineCamera()->Play_Cinematic(L"MantisPunch2");

	m_pPlayer->Play_PlayerSound(L"MantisPunch_Reappear");

	return S_OK;
}

CEvent_MantisPunch_Teleport* CEvent_MantisPunch_Teleport::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_Teleport* pInstance = new CEvent_MantisPunch_Teleport;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_Teleport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_Teleport::Free()
{
	super::Free();
}