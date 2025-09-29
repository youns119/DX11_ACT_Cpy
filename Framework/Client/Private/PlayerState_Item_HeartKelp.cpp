#include "pch.h"
#include "PlayerState_Item_HeartKelp.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Animation.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "Weapon_Player.h"

CPlayerState_Item_HeartKelp::CPlayerState_Item_HeartKelp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_Item_HeartKelp::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
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

void CPlayerState_Item_HeartKelp::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_ITEM_HEARTKELP);

	// 속도 조절 불가능
	// 아이템 사용 활성화
	m_pPlayerStat->bRunnable = false;
	m_pPlayerStat->bUseItem = true;

	m_pModel->SetUp_SlotAnimation(ANIMSLOT::SLOT_UPPER, m_iAnimIndex, false, 0.1f);

	// 플레이어 무기 위치 변경
	if(!m_pModel->Is_SlotAnimActive(ANIMSLOT::SLOT_HAMMER))
		m_pPlayer->Get_PlayerWeapon()->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_FINGER);
	m_pPlayer->Use_HeartKelp();

	m_pPlayer->Play_PlayerSound(L"HeartKelp");
}

void CPlayerState_Item_HeartKelp::Update(_float fTimeDelta)
{
	// 현재 애니메이션이 블렌드 가능 상태 진입시
	// 공중이면 Airbone, 아니면 Idle 로 전환
	if (m_pModel->Get_SlotAnimation(ANIMSLOT::SLOT_UPPER)->Get_Blendable())
	{
		CStateMachine* pStateMachine = m_pPlayer->Get_StateMachine();

		_float fSpeed = m_pMovement->Get_HorizontalSpeed();
		if (fSpeed == 0.f)
		{
			if(m_pPlayerStat->bLockOn)
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

void CPlayerState_Item_HeartKelp::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_ITEM_HEARTKELP);

	// 속도 조절 변수 초기화
	// 아이템 사용 비활성화
	m_pPlayerStat->bRunnable = true;
	m_pPlayerStat->bUseItem = false;
	m_pPlayerStat->bPreserveItem = false;

	// 플레이어 무기 위치 초기화
	if (!m_pModel->Is_SlotAnimActive(ANIMSLOT::SLOT_HAMMER))
		m_pPlayer->Get_PlayerWeapon()->MoveToSocket(CWeapon_Player::SOCKET::SOCKET_HAND);
	m_pPlayer->UnUse_Item();
}

CPlayerState_Item_HeartKelp* CPlayerState_Item_HeartKelp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_Item_HeartKelp* pInstance = new CPlayerState_Item_HeartKelp(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_Item_HeartKelp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Item_HeartKelp::Free()
{
	super::Free();
}