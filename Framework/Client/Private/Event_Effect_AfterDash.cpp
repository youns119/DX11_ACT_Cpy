#include "pch.h"
#include "Event_Effect_AfterDash.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_AfterDash::CEvent_Effect_AfterDash()
{
}

HRESULT CEvent_Effect_AfterDash::Init(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pWeaponCombinedMatrix)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pParentWorldMatrix = pParentWorldMatrix;
	m_pCombinedWorldMatrix = pWeaponCombinedMatrix;

	return S_OK;
}

HRESULT CEvent_Effect_AfterDash::Execute()
{
	_vector vPos = XMVectorSet(m_pCombinedWorldMatrix->_41, m_pCombinedWorldMatrix->_42, m_pCombinedWorldMatrix->_43, 1.f);
	_vector vLook = XMVectorSet(m_pParentWorldMatrix->_31, m_pParentWorldMatrix->_32, m_pParentWorldMatrix->_33, 0.f);
	// CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_ATT, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);
	
	_vector vEffectPos = vPos + vLook * 1.25f;
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_SPRITE_DASH_ATT, vEffectPos, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);

	return S_OK;
}

CEvent_Effect_AfterDash* CEvent_Effect_AfterDash::Create(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pWeaponCombinedMatrix)
{
	CEvent_Effect_AfterDash* pInstance = new CEvent_Effect_AfterDash;

	if (FAILED(pInstance->Init(_pGameObject, pParentWorldMatrix, pWeaponCombinedMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_AfterDash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_AfterDash::Free()
{
	super::Free();
}