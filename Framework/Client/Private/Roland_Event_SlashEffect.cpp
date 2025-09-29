#include "pch.h"
#include "Roland_Event_SlashEffect.h"
#include "GameInstance.h"
#include "Boss_Roland.h"
#include "EffectObject.h"

CRoland_Event_SlashEffect::CRoland_Event_SlashEffect(CGameObject* _pGameObject, _bool bIsLeft)
{
	m_bIsLeft = bIsLeft;
}


HRESULT CRoland_Event_SlashEffect::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	const _float4x4* pParentWorldMatrix = &m_pTransform->Get_WorldMatrix_Ref();
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vUp = m_pTransform->Get_State(CTransform::STATE::UP);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);

	if (true == m_bIsLeft)
	{
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_LINE, vUp * 2.f, vLook, true, pParentWorldMatrix, -20.f, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_DARK, vUp * 2.f, vLook, true, pParentWorldMatrix, -20.f, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_LIGHT, vUp * 2.f, vLook, true, pParentWorldMatrix, -20.f, true);
	}
	else
	{
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_LINE, vUp * 2.f, vLook, true, pParentWorldMatrix, 30.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_DARK, vUp * 2.f, vLook, true, pParentWorldMatrix, 30.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ROLAND_SLASH_MESH_LIGHT, vUp * 2.f, vLook, true, pParentWorldMatrix, 30.f);
	}
	

	return S_OK;
}

CRoland_Event_SlashEffect* CRoland_Event_SlashEffect::Create(CGameObject* _pGameObject, _bool bIsLeft)
{
	CRoland_Event_SlashEffect* pInstance = new CRoland_Event_SlashEffect(_pGameObject, bIsLeft);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CRoland_Event_SlashEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Event_SlashEffect::Free()
{
	__super::Free();
}
