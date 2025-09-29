#include "pch.h"
#include "Roland_Event_BellyFlopEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_BellyFlopEffect::CRoland_Event_BellyFlopEffect(CGameObject* _pGameObject)
{	
}


HRESULT CRoland_Event_BellyFlopEffect::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SLAMSHOCK, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SANDBURST, vPos, vLook, false, nullptr);
	return S_OK;
}

CRoland_Event_BellyFlopEffect* CRoland_Event_BellyFlopEffect::Create(CGameObject* _pGameObject)
{
	CRoland_Event_BellyFlopEffect* pInstance = new CRoland_Event_BellyFlopEffect(_pGameObject);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_BellyFlopEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_BellyFlopEffect::Free()
{
	__super::Free();
}
