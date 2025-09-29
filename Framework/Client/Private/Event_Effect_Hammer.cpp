#include "pch.h"
#include "Event_Effect_Hammer.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_Hammer::CEvent_Effect_Hammer()
{
}

HRESULT CEvent_Effect_Hammer::Init(CGameObject* _pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pSocketMatrix = pSocketMatrix;
	m_iEffectType = iEffectType;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Effect_Hammer::Execute()
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
	case SWAP: // ±³Ã¼(¸ÁÄ¡ ³¡)
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook + vUp * 0.5f, vCamLook, false, nullptr);
		// vPos = CombinedMatrix.r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SWAPSPARK, vPos + vCamLook * 0.25f + vUp * 0.5f, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SWAPDIAMOND, vPos + vCamLook * 0.25f + vUp * 0.5f, vZeroLook, false, nullptr);
		break;
	case CHARGE: // Â÷Â¡(¸ÁÄ¡ ³¡)
		vUp = CombinedMatrix.r[(_uint)CTransform::STATE::UP];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_CHARGING, vUp * 0.75f, vZeroLook, true, m_pSocketMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_ELECTRIC_CHARGING, vUp * 0.75f - vRight * 0.15f, vZeroLook, true, m_pSocketMatrix);

		m_pPlayer->Play_PlayerSound(L"ChargeAttack_Effect");
		m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Voice");

		break;
	case JUMP:	// Á¡ÇÁ(¹ß)
		break;
	case THRUST: // ³»·ÁÂï´Â Áß(Æ®·¹ÀÏ)
		break;
	case THRUST_WALLOP:
		vPos += vLook * 3.f;
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_LIGHTNING, vPos, -vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SANDBURST, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SPARK, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook + vUp * 0.5f, vCamLook, false, nullptr);
		break;
	case CHARGE_WALLOP: // ¹Ù´Ú Âï±â(¸ÁÄ¡ ³¡) - º¯°³ ÄÁ¼ÁÀ¸·Î °¥±î
		vPos += vLook * 3.f;
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_LIGHTNING, vPos, -vLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SANDBURST, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_SPARK, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook + vUp * 0.5f, vCamLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_ELECTRIC_SPREAD, vPos + vUp * 0.35f, vZeroLook, false, nullptr);

		m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Impact");
		m_pPlayer->Play_PlayerSound(L"ChargeAttack_Bubble");
		m_pPlayer->Play_PlayerSound(L"Hammer_ChargeAttack_Voice2");

		break;
	case ONCE:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_LIGHT, vUp * 0.75f, -vLook, true, pWorldMatrix, 0.f, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_DARK, vUp * 0.75f, -vLook, true, pWorldMatrix, 0.f, true);
		break;
	case TWICE:
		vPos += vLook * 3.f;
		pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_DASH_SANDBURST, vPos, vZeroLook, false, nullptr);
		if (nullptr != pEffect)
			static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook * 0.5f - vUp * 0.1f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_TWICELIGHT, vUp * 0.75f, vUp - vRight * 0.5f, true, pWorldMatrix, 0.f, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_TWICEDARK, vUp * 0.75f, vUp - vRight * 0.5f, true, pWorldMatrix, 0.f, true);
		break;
	case DASH:
		vPos += vLook * 3.f;
		pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_DASH_SANDBURST, vPos, vZeroLook, false, nullptr);
		if (nullptr != pEffect)
			static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook * 0.5f - vUp * 0.1f);
		pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HAMMER_DASH_BUBBLE, vPos, vZeroLook, false, nullptr);
		if (nullptr != pEffect)
			static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vLook * 0.5f - vUp * 0.1f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_LIGHT, vUp * 0.75f, -vLook, true, pWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_DARK, vUp * 0.75f, -vLook, true, pWorldMatrix);
		break;
	case AIR: // ÇÑ¹ÙÄû µ¹±â(Æ®·¹ÀÏ)
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_JUMPLIGHT, vZeroPos, vUp - vRight * 0.01f, true, pWorldMatrix, 0.f, true);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_HAMMER_JUMPDARK, vZeroPos, vUp - vRight * 0.01f, true, pWorldMatrix, 0.f, true);
		break;
	default:
		break;
	}

	return S_OK;
}

CEvent_Effect_Hammer* CEvent_Effect_Hammer::Create(CGameObject* _pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType)
{
	CEvent_Effect_Hammer* pInstance = new CEvent_Effect_Hammer;

	if (FAILED(pInstance->Init(_pGameObject, pSocketMatrix, iEffectType)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_Hammer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_Hammer::Free()
{
	super::Free();
}