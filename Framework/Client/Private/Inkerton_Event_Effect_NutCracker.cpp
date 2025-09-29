#include "pch.h"
#include "Inkerton_Event_Effect_NutCracker.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "EffectObject.h"

CInkerton_Event_Effect_NutCracker::CInkerton_Event_Effect_NutCracker(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsReady)
{
	m_pSocketMatrix = _pSocketMatrix;
	m_bIsReady = _bIsReady;
}


HRESULT CInkerton_Event_Effect_NutCracker::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vPos{}, vUp{}, vLook{}, vEffectLook{}, vEffectPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	_bool bIsFlip{ false };
	_float fRotDegree{ 0.f };
	const _float4x4* pWorldMatrix = &m_pTransform->Get_WorldMatrix_Ref();

	// ³Ó Å©·¡Ä¿ ¿­¸± ¶§
	if (true == m_bIsReady)
	{
		// CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE01_PTCL_CIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE02_PTCL_FLOWER, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE03_PTCL_DIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE04_PTCL_CIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE05_PTCL_INNERCIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE06_PTCL_SHIRINKDIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE07_PTCL_FIXEDDIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
	}
	// ³Ó Å©·¡Ä¿ ³»·ÁÄ¥ ¶§
	else
	{
		vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 4.f;
		vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
		vEffectPos = _mat(XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(pWorldMatrix)).position._vector();
		vEffectLook = _mat(XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(pWorldMatrix)).look._vector();
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_NUT00_PTCL_SPLASH, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_NUT01_PTCL_SANDBURST, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_NUT02_PTCL_BUBBLES, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_NUT03_MESHINST_PIECES, vEffectPos, vLook, false, nullptr);
		//CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT09_PTCL_CELEBRATE, vEffectPos, vEffectLook, false, nullptr);
	}

	return S_OK;
}

CInkerton_Event_Effect_NutCracker* CInkerton_Event_Effect_NutCracker::Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsReady)
{
	CInkerton_Event_Effect_NutCracker* pInstance = new CInkerton_Event_Effect_NutCracker(_pGameObject, _pSocketMatrix, _bIsReady);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Effect_NutCracker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Effect_NutCracker::Free()
{
	__super::Free();
}
