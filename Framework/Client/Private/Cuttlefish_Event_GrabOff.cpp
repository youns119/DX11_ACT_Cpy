#include "pch.h"
#include "Cuttlefish_Event_GrabOff.h"

#include "GameInstance.h"

#include "Cuttlefish_GrabTentacle.h"
#include "Player.h"

CCuttlefish_Event_GrabOff::CCuttlefish_Event_GrabOff()
{
}

HRESULT CCuttlefish_Event_GrabOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject))) 
		E_FAIL;	

	m_pGrabTentacle = static_cast<CCuttlefish_GrabTentacle*>(_pGameObject->Find_PartObject({ L"Part_Grab" }));

	return S_OK;
}

HRESULT CCuttlefish_Event_GrabOff::Execute()
{
	CPlayer* pPlayer = nullptr;
	if(nullptr != m_pGrabTentacle->Get_GrabObject())
		pPlayer = static_cast<CPlayer*>(m_pGrabTentacle->Get_GrabObject());

	if (nullptr != pPlayer)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
		CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(g_strTransformTag));
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);		
		_vector vCuttlefishPos = pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vLook = XMVector3Normalize(vPlayerPos - vCuttlefishPos);
		vLook = XMVectorSetY(vLook, XMVectorGetY(vLook) + 0.15f);

		pPlayer->UnGrab(40.f, IDamaged::DAMAGE::DAMAGE_SUPER, m_pGameObject, vLook);
		m_pGrabTentacle->Clear_GrabObject();
	}

	return S_OK;
}

CCuttlefish_Event_GrabOff* CCuttlefish_Event_GrabOff::Create(CGameObject* _pGameObject)
{
	CCuttlefish_Event_GrabOff* pInstance = new CCuttlefish_Event_GrabOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_Event_GrabOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_Event_GrabOff::Free()
{
	super::Free();
}