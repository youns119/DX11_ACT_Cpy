#include "pch.h"
#include "Event_BackStepStart.h"

#include "GameInstance.h"

#include "Player.h"
#include "Movement.h"

#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

CEvent_BackStepStart::CEvent_BackStepStart()
{
}

HRESULT CEvent_BackStepStart::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
	m_pMovement = static_cast<CMovement*>(_pGameObject->Find_Component(L"Com_Movement"));

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_BackStepStart::Execute()
{
	_vec3 vEvadeDir = m_pTransform->Get_State(CTransform::STATE::LOOK);

	m_pMovement->Reset_Velocity();

	if (m_pPlayer->Get_PlayerStat()->bInAir)
	{
		m_pMovement->Set_GravityScale(m_pPlayer->Get_PlayerStat()->fGravityScale * 0.25f);
		m_pMovement->Set_InAirControlRatio(1.f);
	}

	m_pMovement->Set_ForceFriction(m_pPlayer->Get_PlayerStat()->fForceFriction * 3.f);
	m_pMovement->Add_Force(vEvadeDir._vector() * -1.f, 25.f);

	_vector vPos{}, vUp{}, vLook{}, vCamPos{}, vZeroLook{XMVectorSet(0.f, 0.f, 1.f, 0.f)};
	vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	vUp = m_pTransform->Get_State(CTransform::STATE::UP);
	vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	CEffectObject* pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_EVADESANDBURST, vPos, vZeroLook, false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vUp * 0.1f - vLook * 0.2f);

	m_pPlayer->Play_PlayerSound(L"BackStep");
	m_pPlayer->Play_PlayerSound(L"BackStep_Bubble");

	return S_OK;
}

CEvent_BackStepStart* CEvent_BackStepStart::Create(CGameObject* _pGameObject)
{
	CEvent_BackStepStart* pInstance = new CEvent_BackStepStart;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BackStepStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BackStepStart::Free()
{
	super::Free();
}