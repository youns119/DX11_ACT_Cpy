#include "pch.h"
#include "Inkerton_Event_Effect_Execution.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "EffectObject.h"

CInkerton_Event_Effect_Execution::CInkerton_Event_Effect_Execution(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsSlam)
{
	m_pSocketMatrix = _pSocketMatrix;
	m_bIsSlam = _bIsSlam;
}


HRESULT CInkerton_Event_Effect_Execution::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vPos{}, vUp{}, vLook{}, vEffectLook{}, vEffectPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	_bool bIsFlip{ false };
	_float fRotDegree{ 0.f };
	const _float4x4* pWorldMatrix = &m_pTransform->Get_WorldMatrix_Ref();

	if(false == m_bIsSlam)
	{
		vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 4.f;
		vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
		vEffectPos = _mat(XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(pWorldMatrix)).position._vector();
		vEffectLook = _mat(XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(pWorldMatrix)).look._vector();
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SPARK, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PLUNGE_DIAMOND, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PLUNGE_BIGDIAMOND, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_REDBIGONE, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
	}
	else
	{
		vUp = m_pTransform->Get_State(CTransform::STATE::UP);
		vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
		vEffectPos = _mat(XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(pWorldMatrix)).position._vector() + vUp;
		vEffectLook = _mat(XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(pWorldMatrix)).look._vector();
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SPARK, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_DARKIMPACT, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_SANDBURST, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MANTISPUNCH_PTCL_HITBUBBLE, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_NUT03_MESHINST_PIECES, vEffectPos, vLook, false, nullptr);
	}

	return S_OK;
}

CInkerton_Event_Effect_Execution* CInkerton_Event_Effect_Execution::Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsSlam)
{
	CInkerton_Event_Effect_Execution* pInstance = new CInkerton_Event_Effect_Execution(_pGameObject, _pSocketMatrix, _bIsSlam);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Effect_Execution");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Effect_Execution::Free()
{
	__super::Free();
}
