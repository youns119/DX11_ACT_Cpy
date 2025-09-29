#include "pch.h"
#include "Roland_Event_MultiThrustEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_MultiThrustEffect::CRoland_Event_MultiThrustEffect(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	m_pCombinedWorldMatrix = pCombinedWorldMatrix;
}


HRESULT CRoland_Event_MultiThrustEffect::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));

	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ00_WIND, XMVectorSet(0.f, 0.f, 0.f, 1.f), -vLook, true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ01_DARKWIND, XMVectorSet(0.f, 0.f, 0.f, 1.f), -vLook, true, m_pCombinedWorldMatrix);

	CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_PTCL_SEQ03_LINES, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook * 0.1f);

	return S_OK;
}

CRoland_Event_MultiThrustEffect* CRoland_Event_MultiThrustEffect::Create(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	CRoland_Event_MultiThrustEffect* pInstance = new CRoland_Event_MultiThrustEffect(_pGameObject, pCombinedWorldMatrix);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_MultiThrustEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_MultiThrustEffect::Free()
{
	__super::Free();
}
