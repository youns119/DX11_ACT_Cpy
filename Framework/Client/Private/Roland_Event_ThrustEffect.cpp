#include "pch.h"
#include "Roland_Event_ThrustEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_ThrustEffect::CRoland_Event_ThrustEffect(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	m_pCombinedWorldMatrix = pCombinedWorldMatrix;
}


HRESULT CRoland_Event_ThrustEffect::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));

	_vector vCamLook = CGameInstance::GetInstance()->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW).r[(_uint)CTransform::STATE::LOOK];
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ02_EXGLOW, XMVectorSet(0.f, 0.f, 0.f, 1.f), -vCamLook, true, m_pCombinedWorldMatrix);

	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ00_WIND, XMVectorSet(0.f, 0.f, 0.f, 1.f), -vLook, true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ01_DARKWIND, XMVectorSet(0.f, 0.f, 0.f, 1.f), -vLook, true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_MESH_SEQ05_AFTERWIND, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLook, true, m_pCombinedWorldMatrix);

	CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_PTCL_SEQ03_LINES, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook * 0.1f);
	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_THRUST_PTCL_SEQ04_LONGDOTS, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vLook * 0.1f);

	return S_OK;
}

CRoland_Event_ThrustEffect* CRoland_Event_ThrustEffect::Create(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	CRoland_Event_ThrustEffect* pInstance = new CRoland_Event_ThrustEffect(_pGameObject, pCombinedWorldMatrix);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_ThrustEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_ThrustEffect::Free()
{
	__super::Free();
}
