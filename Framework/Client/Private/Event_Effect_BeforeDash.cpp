#include "pch.h"
#include "Event_Effect_BeforeDash.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_BeforeDash::CEvent_Effect_BeforeDash()
{
}

HRESULT CEvent_Effect_BeforeDash::Init(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pWeaponCombinedMatrix)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pParentWorldMatrix = pParentWorldMatrix;
	m_pCombinedWorldMatrix = pWeaponCombinedMatrix;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Effect_BeforeDash::Execute()
{
	_vector vPos = XMVectorSet(m_pCombinedWorldMatrix->_41, m_pCombinedWorldMatrix->_42, m_pCombinedWorldMatrix->_43, 1.f);
	_vector vRight = XMVectorSet(m_pParentWorldMatrix->_11, m_pParentWorldMatrix->_12, m_pParentWorldMatrix->_13, 0.f);
	_vector vUp = XMVectorSet(m_pCombinedWorldMatrix->_21, m_pCombinedWorldMatrix->_22, m_pCombinedWorldMatrix->_23, 0.f);
	_vector vLook = XMVectorSet(m_pParentWorldMatrix->_31, m_pParentWorldMatrix->_32, m_pParentWorldMatrix->_33, 0.f);
	_vector vPivot{}, vEffectPos{};
	vPivot = -vLook - vRight * 0.2f;
	vEffectPos = vPos;
	CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DASH_BEFORE, vEffectPos, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
	if(pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vPivot);

	vEffectPos = vPos - vLook * 0.05f + vRight * 0.25f - vUp * 0.15f;
	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DASH_BEFORE, vEffectPos, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vPivot);

	vEffectPos = vPos - vLook * 0.025f - vRight * 0.15f - vUp * 0.1f;
	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DASH_BEFORE, vEffectPos, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vPivot);
	
	return S_OK;
}

CEvent_Effect_BeforeDash* CEvent_Effect_BeforeDash::Create(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pWeaponCombinedMatrix)
{
	CEvent_Effect_BeforeDash* pInstance = new CEvent_Effect_BeforeDash;

	if (FAILED(pInstance->Init(_pGameObject, pParentWorldMatrix, pWeaponCombinedMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_BeforeDash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_BeforeDash::Free()
{
	super::Free();
}