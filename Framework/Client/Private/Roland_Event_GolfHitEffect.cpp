#include "pch.h"
#include "Roland_Event_GolfHitEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_GolfHitEffect::CRoland_Event_GolfHitEffect(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	m_pCombinedWorldMatrix = pCombinedWorldMatrix;
}


HRESULT CRoland_Event_GolfHitEffect::Execute()
{
	_mat CombinedWorldMatrix = XMLoadFloat4x4(m_pCombinedWorldMatrix);
	_vector vLocalPos = XMVectorSet(0.f, 2.4f, -0.5f, 1.f);
	_vector vCombinedPos = XMVectorSetW(XMVector3TransformCoord(vLocalPos, CombinedWorldMatrix._matrix()), 1.f);
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_LIGHT, vCombinedPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_DARK, vCombinedPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_SANDBURST, vCombinedPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_GOLFHIT_PTCL_SPARK, vCombinedPos, vLook, false, nullptr);
	return S_OK;
}

CRoland_Event_GolfHitEffect* CRoland_Event_GolfHitEffect::Create(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix)
{
	CRoland_Event_GolfHitEffect* pInstance = new CRoland_Event_GolfHitEffect(_pGameObject, pCombinedWorldMatrix);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_GolfHitEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_GolfHitEffect::Free()
{
	__super::Free();
}
