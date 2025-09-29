#include "pch.h"
#include "Inkerton_Event_Effect_ChargeAttack.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "EffectObject.h"

CInkerton_Event_Effect_ChargeAttack::CInkerton_Event_Effect_ChargeAttack(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix)
{
	m_pSocketMatrix = _pSocketMatrix;
}


HRESULT CInkerton_Event_Effect_ChargeAttack::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vEffectLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) }, vCombinedPos{}, vOffsetPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };

	_bool bIsFlip{false};
	_float fRotDegree{0.f};

	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROPEACTION_BLUEBIG, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROPEACTION_DIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGING, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE01_PTCL_CIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE02_PTCL_FLOWER, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE03_PTCL_DIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE04_PTCL_CIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE05_PTCL_INNERCIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE06_PTCL_SHIRINKDIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE07_PTCL_FIXEDDIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());

	return S_OK;
}

CInkerton_Event_Effect_ChargeAttack* CInkerton_Event_Effect_ChargeAttack::Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix)
{
	CInkerton_Event_Effect_ChargeAttack* pInstance = new CInkerton_Event_Effect_ChargeAttack(_pGameObject, _pSocketMatrix);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Effect_ChargeAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Effect_ChargeAttack::Free()
{
	__super::Free();
}
