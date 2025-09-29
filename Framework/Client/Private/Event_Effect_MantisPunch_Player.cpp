#include "pch.h"
#include "Event_Effect_MantisPunch_Player.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_MantisPunch_Player::CEvent_Effect_MantisPunch_Player()
{
}

HRESULT CEvent_Effect_MantisPunch_Player::Init(CGameObject* _pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pSocketMatrix = pSocketMatrix;
	m_iEffectType = iEffectType;

	return S_OK;
}

HRESULT CEvent_Effect_MantisPunch_Player::Execute()
{
	const _float4x4* pWorldMatrix = &static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
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
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_CHARGING, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_HOLDCIRCLE, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_SHRINKCIRCLE, vZeroPos, vZeroLook, true, m_pSocketMatrix, 0.f, false, pWorldMatrix);
		break;
	case JUMP:
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook + vUp * 0.5f, vCamLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_SANDBURST, vPos, vZeroLook, false, nullptr);
		break;
	case TELE_START:
		vPos = (CombinedMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_TELE_START, vZeroPos, vZeroLook, true, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_MANTISPUNCH_TELEPORT, vZeroPos, vLook, true, pWorldMatrix);
		// CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_MANTISPUNCH_WINDIMPACT, vUp * 0.5f, vLook, true, pWorldMatrix);
		break;
	case TELE_END:
		vPos = (CombinedMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_TELE_END, vZeroPos, vZeroLook, true, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_MANTISPUNCH_TELEPORT, vZeroPos, vLook, true, pWorldMatrix);
		// CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_MANTISPUNCH_WINDIMPACT, vZeroPos, vLook, true, pWorldMatrix);
		break;
	case IMPACT:
		vPos = (CombinedMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION] + vUp * 0.5f;
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_DARKIMPACT, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_MANTIS_DIMENSIONBREAK, vPos, vLook, false, nullptr);
		break;
	case JUMP_OUT:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_MANTISPUNCH_SANDBURST, vPos, vZeroLook, false, nullptr);
		break;
	default:
		break;
	}

	return S_OK;
}

CEvent_Effect_MantisPunch_Player* CEvent_Effect_MantisPunch_Player::Create(CGameObject* _pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType)
{
	CEvent_Effect_MantisPunch_Player* pInstance = new CEvent_Effect_MantisPunch_Player;

	if (FAILED(pInstance->Init(_pGameObject, pSocketMatrix, iEffectType)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_MantisPunch_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_MantisPunch_Player::Free()
{
	super::Free();
}