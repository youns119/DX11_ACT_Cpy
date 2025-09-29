#include "pch.h"
#include "BobbitWorm_Event_GrabOff.h"

#include "GameInstance.h"

#include "BobbitWorm_Mouth.h"
#include "Player.h"

CBobbitWorm_Event_GrabOff::CBobbitWorm_Event_GrabOff()
{
}

HRESULT CBobbitWorm_Event_GrabOff::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		E_FAIL;

	m_pMouth = static_cast<CBobbitWorm_Mouth*>(_pGameObject->Find_PartObject({ L"Part_Mouth" }));

	return S_OK;
}

HRESULT CBobbitWorm_Event_GrabOff::Execute()
{
	CPlayer* pPlayer = nullptr;
	if(nullptr != m_pMouth->Get_GrabObject())
		pPlayer = static_cast<CPlayer*>(m_pMouth->Get_GrabObject());

	if (nullptr != pPlayer)
	{
		CTransform* pTransform = static_cast<CTransform*>(pPlayer->Find_Component(g_strTransformTag));
		_vector vLook = pTransform->Get_State(CTransform::STATE::LOOK);

		pPlayer->UnGrab(10.f, IDamaged::DAMAGE::DAMAGE_HEAVY, m_pGameObject);
		m_pMouth->Clear_GrabObject();

		_vector vPos{}, vUp{}, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
		pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
		vPos = pTransform->Get_State(CTransform::STATE::POSITION);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::WORM_PTCL_SANDBURST_EXPL, vPos, vZeroLook, false, nullptr);
	}

	return S_OK;
}

CBobbitWorm_Event_GrabOff* CBobbitWorm_Event_GrabOff::Create(CGameObject* _pGameObject)
{
	CBobbitWorm_Event_GrabOff* pInstance = new CBobbitWorm_Event_GrabOff;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CBobbitWorm_Event_GrabOff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBobbitWorm_Event_GrabOff::Free()
{
	super::Free();
}