#include "pch.h"
#include "Roland_Event_BigSwipeEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_BigSwipeEffect::CRoland_Event_BigSwipeEffect(CGameObject* _pGameObject, _uint _iCombo)
{
	m_iComboCount = _iCombo;
}


HRESULT CRoland_Event_BigSwipeEffect::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vLook{}, vUp{}, vPos{}, vRight{}, vEffectLook{}, vCombinedPos{}, vOffsetPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
	vUp = m_pTransform->Get_State(CTransform::STATE::UP);
	vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	
	const _float4x4* pCombinedWorldMatrix{ nullptr };
	_bool bIsFlip{};
	_float fRotDegree{};
	switch (m_iComboCount)
	{
	case SWIPE_START:
		vOffsetPos = vUp * 2.f;

		bIsFlip = true;
		pCombinedWorldMatrix = &m_pGameObject->Find_PartObject({ L"Part_Wrench_0_R" })->Get_CombinedMatrix();
		vCombinedPos = _mat(*pCombinedWorldMatrix).position._vector();
		vEffectLook = XMVectorSetW(XMVector3Normalize(vCombinedPos - (vPos + vOffsetPos)), 0.f) * -1.f;
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_LINE, vZeroPos, vEffectLook, true, pCombinedWorldMatrix, fRotDegree, bIsFlip);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_LIGHT, vZeroPos, vEffectLook, true, pCombinedWorldMatrix, fRotDegree, bIsFlip);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_DARK, vZeroPos, vEffectLook, true, pCombinedWorldMatrix, fRotDegree, bIsFlip);

		bIsFlip = false;
		pCombinedWorldMatrix = &m_pGameObject->Find_PartObject({ L"Part_Wrench_0_L" })->Get_CombinedMatrix();
		vCombinedPos = _mat(*pCombinedWorldMatrix).position._vector();
		vEffectLook = XMVectorSetW(XMVector3Normalize(vCombinedPos - (vPos + vOffsetPos)), 0.f) * -1.f;
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_LINE, vZeroPos, vEffectLook, true, pCombinedWorldMatrix, fRotDegree, bIsFlip);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_LIGHT, vZeroPos, vEffectLook, true, pCombinedWorldMatrix, fRotDegree, bIsFlip);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_DARK, vZeroPos, vEffectLook, true, pCombinedWorldMatrix, fRotDegree, bIsFlip);

		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_SMALLRING, vUp, vRight, true, &m_pTransform->Get_WorldMatrix_Ref(), fRotDegree, bIsFlip);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGSWIPE_MESH_BIGRING, vUp * 2.f, -vRight, true, &m_pTransform->Get_WorldMatrix_Ref(), fRotDegree, bIsFlip);
		break;

	case SWIPE_END:
		vPos += vUp * 1.5f;
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_MESH_LIGHT, vPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_MESH_DARK, vPos, vLook, false, nullptr);
		break;

	case SWIPE_HIT:
		pCombinedWorldMatrix = &m_pGameObject->Find_PartObject({ L"Part_Wrench_0_R" })->Get_CombinedMatrix();
		vOffsetPos = XMVectorSet(0.f, 2.4f, -0.5f, 1.f);
		vCombinedPos = XMVectorSetW(XMVector3TransformCoord(vOffsetPos, _mat(*pCombinedWorldMatrix)._matrix()), 1.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_LIGHT, vCombinedPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_DARK, vCombinedPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_WALLOP_PTCL_SANDBURST, vCombinedPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_ASHFLOW, vCombinedPos, vLook, false, nullptr);
		break;

	default:
		break;
	}

	return S_OK;
}

CRoland_Event_BigSwipeEffect* CRoland_Event_BigSwipeEffect::Create(CGameObject* _pGameObject, _uint _iCombo)
{
	CRoland_Event_BigSwipeEffect* pInstance = new CRoland_Event_BigSwipeEffect(_pGameObject, _iCombo);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_BigSwipeEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_BigSwipeEffect::Free()
{
	__super::Free();
}
