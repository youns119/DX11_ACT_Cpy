#include "pch.h"
#include "Event_MantisPunch_Retreat.h"

#include "GameObject.h"
#include "Movement.h"

CEvent_MantisPunch_Retreat::CEvent_MantisPunch_Retreat()
{
}

HRESULT CEvent_MantisPunch_Retreat::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	return S_OK;
}

HRESULT CEvent_MantisPunch_Retreat::Execute()
{
	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pMovement->Add_Force(vLook._vector(), -50.f);

	return S_OK;
}

CEvent_MantisPunch_Retreat* CEvent_MantisPunch_Retreat::Create(CGameObject* _pGameObject)
{
	CEvent_MantisPunch_Retreat* pInstance = new CEvent_MantisPunch_Retreat;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_MantisPunch_Retreat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_MantisPunch_Retreat::Free()
{
	super::Free();
}