#include "pch.h"
#include "Inkerton_Event_Effect_Slash.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "EffectObject.h"

CInkerton_Event_Effect_Slash::CInkerton_Event_Effect_Slash(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _uint _iCombo)
{
	m_pSocketMatrix = _pSocketMatrix;
	m_iCombo = _iCombo;
}


HRESULT CInkerton_Event_Effect_Slash::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vEffectLook{}, vEffectPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vUp{};
	const _float4x4* pWorldMatrix = &m_pTransform->Get_WorldMatrix_Ref();
	_bool bIsFlip{ false };
	_float fRotDegree{ 0.f };
	vEffectLook = m_pTransform->Get_State(CTransform::STATE::RIGHT);
	fRotDegree = 12.5f;
	vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 1.5f;
	vEffectPos = m_pTransform->Get_State(CTransform::STATE::POSITION) + vUp;
	switch (m_iCombo)
	{
	case 1:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO1_MESH_MAINBLADE, vUp, vEffectLook, true, pWorldMatrix, fRotDegree, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO1_MESH_LIGHTBLADE, vUp, vEffectLook, true, pWorldMatrix, fRotDegree, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO1_MESH_DARKBLADE, vUp, -vEffectLook, true, pWorldMatrix, -fRotDegree, true);
		break;
	case 2:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_MAINBLADE, vUp, vEffectLook, true, pWorldMatrix, fRotDegree, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_LIGHTBLADE, vUp, vEffectLook, true, pWorldMatrix, fRotDegree, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_DARKBLADE, vUp, -vEffectLook, true, pWorldMatrix, -fRotDegree, true);
		break;
	case 3:
		vEffectLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO3_MESH_MAINBLADE, vUp, vEffectLook, true, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO3_MESH_LIGHTBLADE, vUp, vEffectLook, true, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO3_MESH_DARKBLADE, vUp, vEffectLook, true, pWorldMatrix);
		break;
	case 4:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_MAINBLADE, vUp, -vEffectLook, true, pWorldMatrix, -fRotDegree);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_LIGHTBLADE, vUp, -vEffectLook, true, pWorldMatrix, -fRotDegree);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_DARKBLADE, vUp, vEffectLook, true, pWorldMatrix, fRotDegree);
		break;
	case 5:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_MAINBLADE, vUp, -vEffectLook, true, pWorldMatrix, -fRotDegree);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_LIGHTBLADE, vUp, -vEffectLook, true, pWorldMatrix, -fRotDegree);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_DARKBLADE, vUp, vEffectLook, true, pWorldMatrix, fRotDegree);
		break;
	case 6:
		fRotDegree = 17.5f;
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_MAINBLADE, vUp, -vEffectLook, true, pWorldMatrix, -fRotDegree);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_LIGHTBLADE, vUp, -vEffectLook, true, pWorldMatrix, -fRotDegree);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_COMBO2_MESH_DARKBLADE, vUp, vEffectLook, true, pWorldMatrix, fRotDegree);
		break;
	default:
		break;
	}

	return S_OK;
}

CInkerton_Event_Effect_Slash* CInkerton_Event_Effect_Slash::Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _uint _iCombo)
{
	CInkerton_Event_Effect_Slash* pInstance = new CInkerton_Event_Effect_Slash(_pGameObject, _pSocketMatrix, _iCombo);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Effect_Slash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Effect_Slash::Free()
{
	__super::Free();
}
