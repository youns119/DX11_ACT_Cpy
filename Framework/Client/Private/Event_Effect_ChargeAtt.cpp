#include "pch.h"
#include "Event_Effect_ChargeAtt.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_ChargeAtt::CEvent_Effect_ChargeAtt()
{
}

HRESULT CEvent_Effect_ChargeAtt::Init(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pWeaponCombinedMatrix)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pParentWorldMatrix = pParentWorldMatrix;
	m_pCombinedWorldMatrix = pWeaponCombinedMatrix;

	return S_OK;
}

HRESULT CEvent_Effect_ChargeAtt::Execute()
{
	_vector vPos = XMVectorSet(m_pCombinedWorldMatrix->_41, m_pCombinedWorldMatrix->_42, m_pCombinedWorldMatrix->_43, 1.f);
	_vector vRight = XMVectorSet(m_pParentWorldMatrix->_11, m_pParentWorldMatrix->_12, m_pParentWorldMatrix->_13, 0.f) * 0.25f;
	_vector vUp = XMVectorSet(m_pCombinedWorldMatrix->_21, m_pCombinedWorldMatrix->_22, m_pCombinedWorldMatrix->_23, 0.f) * 0.25f;
	_vector vLook = XMVectorSet(m_pParentWorldMatrix->_31, m_pParentWorldMatrix->_32, m_pParentWorldMatrix->_33, 0.f);
	// CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_ATT, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, m_pCombinedWorldMatrix);

	CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_ATT, vPos + vUp, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook);

	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_ATT, vPos - vUp, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook);

	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_CHARGING_ATT, vPos - vRight, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook);
		
	// 스프링암에 타겟이 설정 되었을 시 해당 타겟을 바라보도록 할 것(위치 지정 후 룩앳)

	return S_OK;
}

CEvent_Effect_ChargeAtt* CEvent_Effect_ChargeAtt::Create(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pWeaponCombinedMatrix)
{
	CEvent_Effect_ChargeAtt* pInstance = new CEvent_Effect_ChargeAtt;

	if (FAILED(pInstance->Init(_pGameObject, pParentWorldMatrix, pWeaponCombinedMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_ChargeAtt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_ChargeAtt::Free()
{
	super::Free();
}