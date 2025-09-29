#include "pch.h"
#include "Event_Effect_Finish.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_Finish::CEvent_Effect_Finish()
{
}

HRESULT CEvent_Effect_Finish::Init(CGameObject* pGameObject, _bool bIsBlood)
{
	if (FAILED(super::Init(pGameObject)))
		return E_FAIL;

	m_bIsBlood = bIsBlood;

	return S_OK;
}

HRESULT CEvent_Effect_Finish::Execute()
{
	const _float4x4* pWorldMatrix = &static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
	_matrix WorldMatrix{};
	_vector vRight{}, vUp{}, vLook{}, vPos{}, vZeroLook{}, vZeroPos{}, vCamLook{};
	_vector vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
	CGameInstance* pGameInstance = { CGameInstance::GetInstance() };
	WorldMatrix = XMLoadFloat4x4(pWorldMatrix);

	vUp = WorldMatrix.r[(_uint)CTransform::STATE::UP];
	vPos = WorldMatrix.r[(_uint)CTransform::STATE::POSITION];

	vZeroLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	vZeroPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	if (m_bIsBlood)
	{
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FINISH_BLOOD, vPos, vZeroLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FINISH_BLOOD_UP, vPos, vZeroLook, false, nullptr);
	}
	else
	{
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_FINISH_CIRCLE, vPos + vCamLook, vCamLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESHINST_FINISH_EXECUTEICON, vPos + vCamLook, vCamLook, false, nullptr);
	}

	return S_OK;
}

CEvent_Effect_Finish* CEvent_Effect_Finish::Create(CGameObject* pGameObject, _bool bIsBlood)
{
	CEvent_Effect_Finish* pInstance = new CEvent_Effect_Finish;

	if (FAILED(pInstance->Init(pGameObject, bIsBlood)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_Finish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_Finish::Free()
{
	super::Free();
}