#include "pch.h"
#include "Event_Effect_Fork.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_Fork::CEvent_Effect_Fork()
{
}

HRESULT CEvent_Effect_Fork::Init(CGameObject* pGameObject, const _float4x4* pCombinedMatrix)
{
	if (FAILED(super::Init(pGameObject)))
		return E_FAIL;

	m_pCombinedMatrix = pCombinedMatrix;

	return S_OK;
}

HRESULT CEvent_Effect_Fork::Execute()
{
	CGameInstance* pGameInstance = { CGameInstance::GetInstance()};
	_vector vPos{}, vZeroLook{};
	vPos = XMLoadFloat4x4(m_pCombinedMatrix).r[(_uint)CTransform::STATE::POSITION];
	vZeroLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FORK_CIRCLE, vPos, vZeroLook, false, nullptr);
	pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FORK_DIAMOND, vPos, vZeroLook, false, nullptr);
	pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FORK_SPREAD, vPos, vZeroLook, false, nullptr);
	pGameInstance->Play_Sound(L"Pickup_Item_High.wav", (_uint)SOUND_CHANNEL::PLAYER_EFFECT, 0.6f);

	return S_OK;
}

CEvent_Effect_Fork* CEvent_Effect_Fork::Create(CGameObject* pGameObject, const _float4x4* pCombinedMatrix)
{
	CEvent_Effect_Fork* pInstance = new CEvent_Effect_Fork;

	if (FAILED(pInstance->Init(pGameObject, pCombinedMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_Fork");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_Fork::Free()
{
	super::Free();
}