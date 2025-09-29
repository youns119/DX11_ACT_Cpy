#include "pch.h"
#include "Inkerton_Event_GrabOff.h"

#include "GameInstance.h"

#include "Inkerton_Nutcracker.h"
#include "Inkerton_StretchedNutcracker.h"
#include "Player.h"

CInkerton_Event_GrabOff::CInkerton_Event_GrabOff()
{
}

HRESULT CInkerton_Event_GrabOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pNutcracker = static_cast<CInkerton_Nutcracker*>(_pGameObject->Find_PartObject({ L"Part_Nutcracker" }));
	m_pStretchedNutcracker = static_cast<CInkerton_StretchedNutcracker*>(_pGameObject->Find_PartObject({ L"Part_StretchedNutcracker" }));

	return S_OK;
}

HRESULT CInkerton_Event_GrabOff::Execute()
{
	CPlayer* pPlayer = nullptr;
	if(nullptr != m_pNutcracker->Get_GrabObject())
		pPlayer = static_cast<CPlayer*>(m_pNutcracker->Get_GrabObject());

	if (nullptr != m_pStretchedNutcracker->Get_GrabObject())
		pPlayer = static_cast<CPlayer*>(m_pStretchedNutcracker->Get_GrabObject());

	if (nullptr != pPlayer)
	{
		//CTransform* pTransform = static_cast<CTransform*>(pPlayer->Find_Component(g_strTransformTag));
		//_vector vLook = pTransform->Get_State(CTransform::STATE::LOOK);

		pPlayer->UnGrab(50.f, IDamaged::DAMAGE::DAMAGE_SUPER, m_pGameObject);

		m_pNutcracker->Clear_GrabObject();
		m_pStretchedNutcracker->Clear_GrabObject();
	}

	return S_OK;
}

CInkerton_Event_GrabOff* CInkerton_Event_GrabOff::Create(CGameObject* _pGameObject)
{
	CInkerton_Event_GrabOff* pInstance = new CInkerton_Event_GrabOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_GrabOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_GrabOff::Free()
{
	super::Free();
}