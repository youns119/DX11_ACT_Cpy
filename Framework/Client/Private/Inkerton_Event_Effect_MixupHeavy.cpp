#include "pch.h"
#include "Inkerton_Event_Effect_MixupHeavy.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "EffectObject.h"

CInkerton_Event_Effect_MixupHeavy::CInkerton_Event_Effect_MixupHeavy(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix)
{
	m_pSocketMatrix = _pSocketMatrix;
}


HRESULT CInkerton_Event_Effect_MixupHeavy::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vEffectLook{}, vEffectPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vUp{};
	_bool bIsFlip{ false };
	_float fRotDegree{ 0.f };

	vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 3.f;
	vEffectLook = m_pTransform->Get_State(CTransform::STATE::RIGHT) * -1.f;
	vEffectPos = m_pTransform->Get_State(CTransform::STATE::POSITION) + vUp;
	const _float4x4* pWorldMatrix = &m_pTransform->Get_WorldMatrix_Ref();
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY00_MESH_MAINBLADE, vUp, vEffectLook, true, pWorldMatrix, -17.5f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY01_MESH_LIGHTLINEBLADE, vUp, vEffectLook, true, pWorldMatrix, -17.5f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY02_MESH_DARKTRAIL, vUp, -vEffectLook, true, pWorldMatrix, 17.5f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY03_MESH_GLOW, vUp, vEffectLook, true, pWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY04_MESH_WIND, vEffectPos, vEffectLook, false, nullptr);
	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY05_MESH_SMALLRING, vEffectPos, vEffectLook, false, nullptr, true);
	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_HEAVY06_MESH_BIGRING, vEffectPos, -vEffectLook, false, nullptr, true);

	return S_OK;
}

CInkerton_Event_Effect_MixupHeavy* CInkerton_Event_Effect_MixupHeavy::Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix)
{
	CInkerton_Event_Effect_MixupHeavy* pInstance = new CInkerton_Event_Effect_MixupHeavy(_pGameObject, _pSocketMatrix);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Effect_MixupHeavy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Effect_MixupHeavy::Free()
{
	__super::Free();
}
