#include "pch.h"
#include "Event_Effect_FallDown.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"
#include "Player.h"

CEvent_Effect_FallDown::CEvent_Effect_FallDown()
{
}

HRESULT CEvent_Effect_FallDown::Init(CGameObject* pGameObject)
{
	if (FAILED(super::Init(pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvent_Effect_FallDown::Execute()
{
	const _float4x4* pWorldMatrix = &static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
	_vector vPos{}, vZeroLook{};
	vPos = XMLoadFloat4x4(pWorldMatrix).r[(_uint)CTransform::STATE::POSITION];
	vZeroLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPSANDBURST, vPos, vZeroLook, false, nullptr);

	return S_OK;
}

CEvent_Effect_FallDown* CEvent_Effect_FallDown::Create(CGameObject* pGameObject)
{
	CEvent_Effect_FallDown* pInstance = new CEvent_Effect_FallDown;

	if (FAILED(pInstance->Init(pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Effect_FallDown");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Effect_FallDown::Free()
{
	super::Free();
}