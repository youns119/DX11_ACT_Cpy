#include "pch.h"
#include "Roland_Event_WallopEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_WallopEffect::CRoland_Event_WallopEffect(CGameObject* _pGameObject, _bool _bIsHit, const _float4x4* _pCombinedWorldMatrix)
{
	m_pCombinedWorldMatrix = _pCombinedWorldMatrix;
	m_bIsHitEffect = _bIsHit;
}


HRESULT CRoland_Event_WallopEffect::Execute()
{
	if (false == m_bIsHitEffect)
	{
		CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
		_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
		_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
		_vector vUp = m_pTransform->Get_State(CTransform::STATE::UP);
		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION) + vUp * 1.5f + vRight * 0.75f;
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_MESH_LIGHT, vPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_MESH_DARK, vPos, vLook, false, nullptr);
	}
	else
	{
		_mat CombinedWorldMatrix = XMLoadFloat4x4(m_pCombinedWorldMatrix);
		_vector vLocalPos = XMVectorSet(0.f, 2.4f, -0.5f, 1.f);
		_vector vCombinedPos = XMVectorSetW(XMVector3TransformCoord(vLocalPos, CombinedWorldMatrix._matrix()), 1.f);
		_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_LIGHT, vCombinedPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_DARK, vCombinedPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_SANDBURST, vCombinedPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_ASHFLOW, vCombinedPos, vLook, false, nullptr);
	}

	return S_OK;
}

CRoland_Event_WallopEffect* CRoland_Event_WallopEffect::Create(CGameObject* _pGameObject, _bool _bIsHit, const _float4x4* _pCombinedWorldMatrix)
{
	CRoland_Event_WallopEffect* pInstance = new CRoland_Event_WallopEffect(_pGameObject, _bIsHit, _pCombinedWorldMatrix);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_WallopEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_WallopEffect::Free()
{
	__super::Free();
}
