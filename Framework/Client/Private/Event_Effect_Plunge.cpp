#include "pch.h"
#include "Event_Effect_Plunge.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_Plunge::CEvent_Effect_Plunge()
{
}

HRESULT CEvent_Effect_Plunge::Init(CGameObject* pGameObject, const _float4x4* pSocketMatrix, _bool bIsImpact)
{
	if (FAILED(super::Init(pGameObject)))
		return E_FAIL;

	m_pSocketMatrix = pSocketMatrix;
	m_bIsImpact = bIsImpact;

	return S_OK;
}

HRESULT CEvent_Effect_Plunge::Execute()
{
	const _float4x4* pWorldMatrix = &static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
	_matrix WorldMatrix{};
	_vector vRight{}, vUp{}, vLook{}, vPos{}, vZeroLook{}, vZeroPos{}, vCamLook{};
	_vector vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
	CEffectObject* pEffect = { nullptr };
	WorldMatrix = XMLoadFloat4x4(pWorldMatrix);

	vRight = WorldMatrix.r[(_uint)CTransform::STATE::RIGHT];
	vUp = WorldMatrix.r[(_uint)CTransform::STATE::UP];
	vLook = WorldMatrix.r[(_uint)CTransform::STATE::LOOK];
	vPos = WorldMatrix.r[(_uint)CTransform::STATE::POSITION];

	vZeroLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	vZeroPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	if (m_bIsImpact)
	{
		vPos += vLook * 3.f;
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_LIGHTNING, vPos, -vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SANDBURST, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook + vUp * 0.5f, vCamLook, false, nullptr);


		_bool bIsHammer = static_cast<CPlayer*>(m_pGameObject)->Get_PlayerStat_Const()->bHasHammer;
		if (true == bIsHammer)
			CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SPARK, vPos, vZeroLook, false, nullptr);
		else
			CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_SPARK, vPos, vZeroLook, false, nullptr);
	}
	else
	{
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PLUNGE_BIGDIAMOND, vZeroPos, vZeroLook, true, m_pSocketMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PLUNGE_DIAMOND, vZeroPos, vZeroLook, true, m_pSocketMatrix);
	}

	return S_OK;
}

CEvent_Effect_Plunge* CEvent_Effect_Plunge::Create(CGameObject* pGameObject, const _float4x4* pSocketMatrix, _bool bIsImpact)
{
	CEvent_Effect_Plunge* pInstance = new CEvent_Effect_Plunge;

	if (FAILED(pInstance->Init(pGameObject, pSocketMatrix, bIsImpact)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_Plunge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_Plunge::Free()
{
	super::Free();
}