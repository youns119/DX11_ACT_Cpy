#include "pch.h"
#include "Event_Effect_MantisPunch_Topoda.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_MantisPunch_Topoda::CEvent_Effect_MantisPunch_Topoda()
{
}

HRESULT CEvent_Effect_MantisPunch_Topoda::Init(CGameObject* _pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pSocketMatrix = pSocketMatrix;
	m_iEffectType = iEffectType;

	return S_OK;
}

HRESULT CEvent_Effect_MantisPunch_Topoda::Execute()
{
	const _float4x4* pWorldMatrix = &m_pGameObject->Get_CombinedMatrix();
	_matrix CombinedMatrix{}, WorldMatrix{};
	_vector vRight{}, vUp{}, vLook{}, vPos{}, vZeroLook{}, vZeroPos{}, vCamLook{};
	_vector vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
	CEffectObject* pEffect = { nullptr };
	WorldMatrix = XMLoadFloat4x4(pWorldMatrix);
	CombinedMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	vRight = WorldMatrix.r[(_uint)CTransform::STATE::RIGHT];
	vUp = WorldMatrix.r[(_uint)CTransform::STATE::UP];
	vLook = WorldMatrix.r[(_uint)CTransform::STATE::LOOK];
	vPos = WorldMatrix.r[(_uint)CTransform::STATE::POSITION];

	vZeroLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	vZeroPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	switch (m_iEffectType)
	{
	case CHARGE:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_CHARGING, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_HOLDCIRCLE, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_SHRINKCIRCLE, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
		break;
	case TELE_START:
		vPos = (CombinedMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_TELE_START, vZeroPos, vZeroLook, true, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_MESH_MANTISPUNCH_TELEPORT, vZeroPos, vLook, true, pWorldMatrix);
		// CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_MESH_MANTISPUNCH_WINDIMPACT, vUp * 0.5f, vLook, true, pWorldMatrix);
		break;
	case TELE_END:
		vPos = (CombinedMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_TELE_END, vZeroPos, vZeroLook, true, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_MESH_MANTISPUNCH_TELEPORT, vZeroPos, vLook, true, pWorldMatrix);
		// CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_MANTISPUNCH_WINDIMPACT, vZeroPos, vLook, true, pWorldMatrix);
		break;
	case SLASH:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_LIGHT, vUp * 0.75f - vRight, -vLook, true, pWorldMatrix, 0.f, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_DARK, vUp * 0.75f - vRight, -vLook, true, pWorldMatrix, 0.f, true);
		break;
	case IMPACT:
		vPos = (CombinedMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION] + vUp;
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_PTCL_MANTISPUNCH_DARKIMPACT, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::TOPODA_MESHINST_MANTISPUNCH_DIMENSIONBREAK, vPos, vLook, false, nullptr);

		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MANTISPUNCH_MESHINST_GLASSBREAK, vPos, vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MANTISPUNCH_PTCL_HITBUBBLE, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MANTISPUNCH_PTCL_HITSPARK, vPos, vZeroLook, false, nullptr);

		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook + vUp * 0.5f, vCamLook, false, nullptr);
		break;
	case JUMP_OUT:
		vPos += vLook * 3.f;
		pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_DASH_SANDBURST, vPos, vZeroLook, false, nullptr);
		if (nullptr != pEffect)
			static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook * 0.5f - vUp * 0.1f);
		pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_DASH_BUBBLE, vPos, vZeroLook, false, nullptr);
		if (nullptr != pEffect)
			static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook * 0.5f - vUp * 0.1f);
		break;
	default:
		break;
	}

	return S_OK;
}

CEvent_Effect_MantisPunch_Topoda* CEvent_Effect_MantisPunch_Topoda::Create(CGameObject* _pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType)
{
	CEvent_Effect_MantisPunch_Topoda* pInstance = new CEvent_Effect_MantisPunch_Topoda;

	if (FAILED(pInstance->Init(_pGameObject, pSocketMatrix, iEffectType)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_MantisPunch_Topoda");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_MantisPunch_Topoda::Free()
{
	super::Free();
}