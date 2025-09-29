#include "pch.h"
#include "PlayerState_Debuff_Fear.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"

#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

CPlayerState_Debuff_Fear::CPlayerState_Debuff_Fear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Debuff_Fear::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Drink");

	return S_OK;
}

void CPlayerState_Debuff_Fear::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_DEBUFF_FEAR);

	// 속도 조절 불가능
	// 아이템 사용 활성화
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bUseItem = true;

	Call_FearEffect();
	m_pModel->SetUp_SlotAnimation(ANIMSLOT::SLOT_UPPER, m_iAnimIndex, false, 0.1f);
}

void CPlayerState_Debuff_Fear::Update(_float fTimeDelta)
{
	// 현재 애니메이션이 블렌드 가능 상태 진입시
// 공중이면 Airbone, 아니면 Idle 로 전환
	if (m_pModel->Get_SlotAnimation(ANIMSLOT::SLOT_UPPER)->Get_Blendable())
	{
		CStateMachine* pStateMachine = m_pPlayer->Get_StateMachine();

		_float fSpeed = m_pMovement->Get_HorizontalSpeed();
		if (fSpeed == 0.f)
		{
			if (m_pPlayerStat->bLockOn)
				pStateMachine->ChangeState(L"LockOn");
			else pStateMachine->ChangeState(L"Idle");
		}
		else
		{
			if (m_pPlayerStat->bLockOn)
				pStateMachine->ChangeState(L"LockOn");
			else pStateMachine->ChangeState(L"Walk");
		}

		m_pOwner->ChangeState(L"None");
	}
}

void CPlayerState_Debuff_Fear::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_DEBUFF_FEAR);

	// 속도 조절 변수 초기화
	// 아이템 사용 비활성화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bUseItem = false;
	Disable_FearEffect();
}

void CPlayerState_Debuff_Fear::Call_FearEffect()
{
	if (nullptr != m_pTearEffect)
		Disable_FearEffect();

	_vector vOffsetPos{}, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	vOffsetPos = pPlayerTransform->Get_State(CTransform::STATE::UP) * 1.5f;
	
	m_pTearEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_DEBUF_FEAR, vOffsetPos, vZeroLook, true, &pPlayerTransform->Get_WorldMatrix_Ref());
}

void CPlayerState_Debuff_Fear::Disable_FearEffect()
{
	if (nullptr != m_pTearEffect)
		m_pTearEffect->Clear_Info(0.f);
}

CPlayerState_Debuff_Fear* CPlayerState_Debuff_Fear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Debuff_Fear* pInstance = new CPlayerState_Debuff_Fear(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Debuff_Fear");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Debuff_Fear::Free()
{
	super::Free();
}