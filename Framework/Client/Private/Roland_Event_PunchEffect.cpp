#include "pch.h"
#include "Roland_Event_PunchEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_PunchEffect::CRoland_Event_PunchEffect(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	m_pCombinedWorldMatrix = pCombinedWorldMatrix;
}


HRESULT CRoland_Event_PunchEffect::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ00_0, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ00_0, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ00_0, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ00_0, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ01, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_MESH_PUNCH_SEQ02, XMVectorSet(0.f, 0.f, 0.f, 1.f), -vLook, true, m_pCombinedWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_MESH_PUNCH_SEQ03, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLook, true, m_pCombinedWorldMatrix);
	CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SPRT_PUNCH_SEQ04, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	if (pEffect != nullptr)
	{
		_vector vCamLook = CGameInstance::GetInstance()->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW).r[(_uint)CTransform::STATE::LOOK];
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vCamLook);
	}

	_vector vUp = m_pTransform->Get_State(CTransform::STATE::UP);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_MESH_PUNCH_SEQ05, vPos + vUp * 2.f + vLook * 2.f, -vLook, false, nullptr);
	return S_OK;
}

CRoland_Event_PunchEffect* CRoland_Event_PunchEffect::Create(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	CRoland_Event_PunchEffect* pInstance = new CRoland_Event_PunchEffect(_pGameObject, pCombinedWorldMatrix);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_PunchEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_PunchEffect::Free()
{
	__super::Free();
}
