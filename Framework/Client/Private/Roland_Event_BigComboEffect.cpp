#include "pch.h"
#include "Roland_Event_BigComboEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_BigComboEffect::CRoland_Event_BigComboEffect(CGameObject* _pGameObject, const _float4x4* _pCombinedWorldMatrix, _uint _iComboCount)
{
	m_pCombinedWorldMatrix = _pCombinedWorldMatrix;
	m_iComboCount = _iComboCount;
}


HRESULT CRoland_Event_BigComboEffect::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_matrix ParentWorldMatrix = _mat(m_pTransform->Get_WorldMatrix_Ref())._matrix();
	_matrix CombinedWorldMatrix = _mat(*m_pCombinedWorldMatrix)._matrix();
	_vector vUp{}, vPos{}, vRight{}, vEffectLook{}, vCombinedPos{}, vOffsetPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	vPos = ParentWorldMatrix.r[_uint(CTransform::STATE::POSITION)];
	vUp = ParentWorldMatrix.r[_uint(CTransform::STATE::UP)];
	vRight = ParentWorldMatrix.r[_uint(CTransform::STATE::RIGHT)];

	// vLook = ParentWorldMatrix.r[_uint(CTransform::STATE::LOOK)];
	vCombinedPos = CombinedWorldMatrix.r[_uint(CTransform::STATE::POSITION)];

	_bool bIsFlip{};
	_float fRotDegree{};

	switch (m_iComboCount)
	{
	case 0:
		bIsFlip = false;
		vOffsetPos = vUp;
		fRotDegree = -5.f;
		break;
	case 1:
		bIsFlip = false;
		vOffsetPos = vUp * 2.f;
		fRotDegree = -5.f;
		break;
	case 2:
		bIsFlip = true;
		vOffsetPos = vUp * 0.75f;
		fRotDegree = 5.f;
		break;
	case 3:
		bIsFlip = true;
		vOffsetPos = vUp * 0.5f;
		fRotDegree = 5.f;
		break;
	default:
		break;
	}

	vEffectLook = XMVectorSetW(XMVector3Normalize(vCombinedPos - (vPos + vOffsetPos)), 0.f) * -1.f;

	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_LINE, vZeroPos, vEffectLook, true, m_pCombinedWorldMatrix, fRotDegree, bIsFlip);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_LIGHT, vZeroPos, vEffectLook, true, m_pCombinedWorldMatrix, fRotDegree, bIsFlip);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_DARK, vZeroPos, vEffectLook, true, m_pCombinedWorldMatrix, fRotDegree, bIsFlip);

	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_LINE, vCombinedPos, vEffectLook, false, nullptr, fRotDegree, bIsFlip);
	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_LIGHT, vCombinedPos, vEffectLook, false, nullptr, fRotDegree, bIsFlip);
	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_BIGCOMBO_MESH_DARK, vCombinedPos, vEffectLook, false, nullptr, fRotDegree, bIsFlip);

	return S_OK;
}

CRoland_Event_BigComboEffect* CRoland_Event_BigComboEffect::Create(CGameObject* _pGameObject, const _float4x4* _pCombinedWorldMatrix, _uint _iComboCount)
{
	CRoland_Event_BigComboEffect* pInstance = new CRoland_Event_BigComboEffect(_pGameObject, _pCombinedWorldMatrix, _iComboCount);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_BigComboEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_BigComboEffect::Free()
{
	__super::Free();
}
