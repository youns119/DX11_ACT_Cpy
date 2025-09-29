#include "pch.h"
#include "Event_Urchin_Dodge.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Movement.h"

#include "Player.h"

CEvent_Urchin_Dodge::CEvent_Urchin_Dodge()
{
}

HRESULT CEvent_Urchin_Dodge::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Urchin_Dodge::Execute()
{
	m_pMovement->Reset_Velocity();

	_vec3 vLook = m_pTransform->Get_State(CTransform::STATE::LOOK) * -1.f;

	m_pMovement->Add_Force(vLook._vector(), 20.f);

	return S_OK;
}

CEvent_Urchin_Dodge* CEvent_Urchin_Dodge::Create(CGameObject* _pGameObject)
{
	CEvent_Urchin_Dodge* pInstance = new CEvent_Urchin_Dodge;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Urchin_Dodge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Urchin_Dodge::Free()
{
	super::Free();
}