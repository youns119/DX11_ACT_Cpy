#include "pch.h"
#include "Event_JumpLaunchStart.h"

#include "Player.h"
#include "Movement.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

CEvent_JumpLaunchStart::CEvent_JumpLaunchStart()
{
}

HRESULT CEvent_JumpLaunchStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));
	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_JumpLaunchStart::Execute()
{
	_vector vPos{}, vUp{}, vLook{}, vCamPos{};
	vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	vUp = m_pTransform->Get_State(CTransform::STATE::UP);
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos + vUp, vLook, false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vUp * 0.001f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPSANDBURST, vPos, vLook, false, nullptr);

	vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
 	vLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos, vLook, false, nullptr);

	m_pMovement->Add_Force(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pPlayerStat->fJumpForce);
	return S_OK;
}

CEvent_JumpLaunchStart* CEvent_JumpLaunchStart::Create(CGameObject* _pGameObject)
{
	CEvent_JumpLaunchStart* pInstance = new CEvent_JumpLaunchStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_JumpLaunchStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_JumpLaunchStart::Free()
{
	super::Free();
}