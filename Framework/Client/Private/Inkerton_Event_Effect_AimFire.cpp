#include "pch.h"
#include "Inkerton_Event_Effect_AimFire.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "EffectObject.h"

CInkerton_Event_Effect_AimFire::CInkerton_Event_Effect_AimFire(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsFire)
{
	m_pSocketMatrix = _pSocketMatrix;
	m_bIsFire = _bIsFire;
}


HRESULT CInkerton_Event_Effect_AimFire::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vPos{}, vUp{}, vLook{}, vEffectLook{}, vEffectPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	_bool bIsFlip{ false };
	_float fRotDegree{ 0.f };
	const _float4x4* pWorldMatrix = &m_pTransform->Get_WorldMatrix_Ref();

	if (false == m_bIsFire)
	{
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT00_PTCL_DIAMOND, vZeroPos, vZeroLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT01_PTCL_BIGSPREADDIAMOND, vZeroPos, vZeroLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT02_PTCL_SHRINKCIRCLE, vZeroPos, vZeroLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT03_PTCL_INNERCIRCLE, vZeroPos, vZeroLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT04_PTCL_EXPANDCIRCLE, vZeroPos, vZeroLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT05_PTCL_CIRCLE, vZeroPos, vZeroLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT06_MESH_CHARGINGLASER, vZeroPos, vZeroLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, pWorldMatrix);
	}
	else
	{		
		vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 4.f;
		vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
		vEffectPos = _mat(XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(pWorldMatrix)).position._vector();
		vEffectLook = _mat(XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(pWorldMatrix)).look._vector();
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vEffectPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT07_MESH_LASER, vEffectPos, vLook, false, nullptr);
		CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT09_PTCL_CELEBRATE, vEffectPos, vEffectLook, false, nullptr);
		if (pEffect == nullptr)
			return S_OK;
		// static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vEffectPos - vEffectLook);
	}

	return S_OK;
}

CInkerton_Event_Effect_AimFire* CInkerton_Event_Effect_AimFire::Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsFire)
{
	CInkerton_Event_Effect_AimFire* pInstance = new CInkerton_Event_Effect_AimFire(_pGameObject, _pSocketMatrix, _bIsFire);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_Effect_AimFire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_Effect_AimFire::Free()
{
	__super::Free();
}
