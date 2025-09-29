#include "pch.h"
#include "Roland_Event_GolfEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_GolfEffect::CRoland_Event_GolfEffect(CGameObject* _pGameObject)
{

}


HRESULT CRoland_Event_GolfEffect::Execute()
{
	CTransform* pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	
	_vector vPos{}, vLook{};
	vPos = pTransform->Get_State(CTransform::STATE::POSITION);
	vLook = pTransform->Get_State(CTransform::STATE::LOOK) * -1.f;

	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_GOLF_MESH_DARK, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_GOLF_MESH_LIGHT, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_GOLF_MESH_LINE, vPos, vLook, false, nullptr);

	return S_OK;
}

CRoland_Event_GolfEffect* CRoland_Event_GolfEffect::Create(CGameObject* _pGameObject)
{
	CRoland_Event_GolfEffect* pInstance = new CRoland_Event_GolfEffect(_pGameObject);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_GolfEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_GolfEffect::Free()
{
	__super::Free();
}
