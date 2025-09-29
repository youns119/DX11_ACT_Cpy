#include "pch.h"
#include "StateMachine_PlayerUpper.h"

#include "GameObject.h"
#include "Model.h"
#include "Animation.h"

#include "Player.h"

#include "PlayerState_None.h"
#include "PlayerState_Guard.h"
#include "PlayerState_Grapple_Aim.h"
#include "PlayerState_Item_HeartKelp.h"

CStateMachine_PlayerUpper::CStateMachine_PlayerUpper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CStateMachine_PlayerUpper::CStateMachine_PlayerUpper(const CStateMachine_PlayerUpper& Prototype)
	: super(Prototype)
{
}

HRESULT CStateMachine_PlayerUpper::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMachine_PlayerUpper::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	super::DESC superDesc = pDesc->StateMachineDesc;

	m_pPlayerState = pDesc->pPlayerState;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	m_pModel = static_cast<CModel*>(m_pOwner->Find_Component(L"Com_Model"));

	m_pPlayer = static_cast<CPlayer*>(m_pOwner);

	return S_OK;
}

void CStateMachine_PlayerUpper::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

HRESULT CStateMachine_PlayerUpper::ChangeState(const _wstring& strState)
{
	_bool bCanChange = Check_AnimGraph(strState);

	if (bCanChange)
		if (FAILED(super::ChangeState(strState)))
			return E_FAIL;

	return S_OK;
}

_bool CStateMachine_PlayerUpper::Check_AnimGraph(const _wstring& strAnimState)
{
	_bool bTransition{};

	if (m_pModel->Get_SlotAnimation(ANIMSLOT::SLOT_UPPER) != nullptr)
		bTransition = m_pPlayer->Get_PlayerStat()->bTransition;

	if (strAnimState == L"None") return true;

	if (m_pPlayerState->test(PLAYERSTATE_DEBUFF_FEAR))
	{
		if (strAnimState == L"Guard") return true;
	}

	// Guard
	if (m_pPlayerState->test(PLAYERSTATE_GUARD))
	{
	}
	// Grapple_Aim
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_AIM))
	{
		if (strAnimState == L"Guard") return true;
	}
	// Item_HeartKelp
	else if (m_pPlayerState->test(PLAYERSTATE_ITEM_HEARTKELP))
	{
	}
	// Item_HeartKelp
	else if (m_pPlayerState->test(PLAYERSTATE_DEBUFF_FEAR))
	{
	}
	// Idle
	else if (m_pPlayerState->test(PLAYERSTATE_IDLE))
	{
		if (strAnimState == L"Guard") return true;
		else if (strAnimState == L"Grapple_Aim") return true;
		else if (strAnimState == L"Item_HeartKelp") return true;
	}
	// Walk
	else if (m_pPlayerState->test(PLAYERSTATE_WALK))
	{
		if (strAnimState == L"Guard") return true;
		else if (strAnimState == L"Grapple_Aim") return true;
		else if (strAnimState == L"Item_HeartKelp") return true;
	}
	// Run
	else if (m_pPlayerState->test(PLAYERSTATE_RUN))
	{
		if (strAnimState == L"Guard") return true;
		else if (strAnimState == L"Grapple_Aim") return true;
		else if (strAnimState == L"Item_HeartKelp") return true;
	}
	// Airbone
	else if (m_pPlayerState->test(PLAYERSTATE_AIRBONE))
	{
		if (strAnimState == L"Grapple_Aim") return true;
	}
	// Swimming
	else if (m_pPlayerState->test(PLAYERSTATE_SWIMMING))
	{
		if (strAnimState == L"Grapple_Aim") return true;
	}
	// Rising
	else if (m_pPlayerState->test(PLAYERSTATE_RISING))
	{
		if (strAnimState == L"Grapple_Aim") return true;
	}
	// LockOn
	else if (m_pPlayerState->test(PLAYERSTATE_LOCKON))
	{
		if (strAnimState == L"Guard") return true;
		else if (strAnimState == L"Grapple_Aim") return true;
		else if (strAnimState == L"Item_HeartKelp") return true;
	}
	// JumpLaunch
	else if (m_pPlayerState->test(PLAYERSTATE_JUMPLAUNCH))
	{
		if (strAnimState == L"Grapple_Aim") return true;
	}
	// JumpLand
	else if (m_pPlayerState->test(PLAYERSTATE_JUMPLAND))
	{
		if (strAnimState == L"Guard") return true;
		else if (strAnimState == L"Grapple_Aim") return true;
		else if (strAnimState == L"Item_HeartKelp") return true;
	}
	// JumpLaunch
	else if (m_pPlayerState->test(PLAYERSTATE_BACKFLIP))
	{
	}
	// Evade
	else if (m_pPlayerState->test(PLAYERSTATE_EVADE))
	{
	}
	// BackStep
	else if (m_pPlayerState->test(PLAYERSTATE_BACKSTEP))
	{
	}
	// NormalAttack
	else if (m_pPlayerState->test(PLAYERSTATE_NORMALATTACK))
	{
	}
	// ChargeAttack
	else if (m_pPlayerState->test(PLAYERSTATE_CHARGEATTACK))
	{
	}
	// DashAttack
	else if (m_pPlayerState->test(PLAYERSTATE_DASHATTACK))
	{
	}
	// SwipeAttack
	else if (m_pPlayerState->test(PLAYERSTATE_SWIPEATTACK))
	{
	}
	// PlungeAttack
	else if (m_pPlayerState->test(PLAYERSTATE_PLUNGEATTACK))
	{
	}
	// ParryAttack
	else if (m_pPlayerState->test(PLAYERSTATE_PARRYATTACK))
	{
	}
	// BackStepAttack
	else if (m_pPlayerState->test(PLAYERSTATE_BACKSTEPATTACK))
	{
	}
	// Parry
	else if (m_pPlayerState->test(PLAYERSTATE_PARRY))
	{
		if (strAnimState == L"Guard") return true;
		else if (strAnimState == L"Grapple_Aim") return true;
		else if (strAnimState == L"Item_HeartKelp") return true;
	}
	// ShellEquip
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLEQUIP))
	{
	}
	// ShellEnter
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLENTER))
	{
	}
	// ShellIdle
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLIDLE))
	{
	}
	// ShellCrawl
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLCRAWL))
	{
	}
	// ShellParry
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLPARRY))
	{
		if (strAnimState == L"Guard") return true;
		else if (strAnimState == L"Grapple_Aim") return true;
		else if (strAnimState == L"Item_HeartKelp") return true;
	}
	// ShellAttacked
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLATTACKED))
	{
	}
	// ShellSkill_Fizzle
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLSKILL_FIZZLE))
	{
	}
	// ShellSkill_RollOut
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLSKILL_ROLLOUT))
	{
	}
	// Grapple_Throw
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_THROW))
	{
	}
	// Grapple_Loop
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_LOOP))
	{
	}
	// Grabbed
	else if (m_pPlayerState->test(PLAYERSTATE_GRABBED))
	{
	}
	// NormalAttacked
	else if (m_pPlayerState->test(PLAYERSTATE_NORMALATTACKED))
	{
		if (strAnimState == L"Guard" && bTransition) return true;
		else if (strAnimState == L"Item_HeartKelp" && bTransition) return true;
	}
	// HeavyAttacked_Front
	else if (m_pPlayerState->test(PLAYERSTATE_HEAVYATTACKED_FRONT))
	{
	}
	// HeavyAttacked_Back
	else if (m_pPlayerState->test(PLAYERSTATE_HEAVYATTACKED_BACK))
	{
	}
	// GetUp_Front
	else if (m_pPlayerState->test(PLAYERSTATE_GETUP_FRONT))
	{
		if (strAnimState == L"Guard" && bTransition) return true;
		else if (strAnimState == L"Item_HeartKelp" && bTransition) return true;
	}
	// GetUp_Back
	else if (m_pPlayerState->test(PLAYERSTATE_GETUP_BACK))
	{
		if (strAnimState == L"Guard" && bTransition) return true;
		else if (strAnimState == L"Item_HeartKelp" && bTransition) return true;
	}
	// 
	else if (m_pPlayerState->test(PLAYERSTATE_SUPERATTACKED))
	{
		if (strAnimState == L"Guard" && bTransition) return true;
		else if (strAnimState == L"Item_HeartKelp" && bTransition) return true;
		}
	// NormalDeath
	else if (m_pPlayerState->test(PLAYERSTATE_NORMALDEATH))
	{
	}
	// HeavyDeath_Front
	else if (m_pPlayerState->test(PLAYERSTATE_HEAVYDEATH_FRONT))
	{
	}
	// HeavyDeath_Back
	else if (m_pPlayerState->test(PLAYERSTATE_HEAVYDEATH_BACK))
	{
	}
	// HammerPick
	else if (m_pPlayerState->test(PLAYERSTATE_HAMMER_PICK))
	{
		if (strAnimState == L"Guard" && bTransition) return true;
		else if (strAnimState == L"Item_HeartKelp" && bTransition) return true;
	}
	// Grapple_AttackStart
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_ATTACKSTART))
	{
	}
	// Grapple_Attack
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_ATTACK))
	{
	}
	// Grapple_AttackEnd
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_ATTACKEND))
	{
	}
	// FinishAttack
	else if (m_pPlayerState->test(PLAYERSTATE_FINISHATTACK))
	{
	}
	// ActiveSkill_MantisPunch
	else if (m_pPlayerState->test(PLAYERSTATE_MANTISPUNCH))
	{
	}
	// ActiveSkill_Urchin
	else if (m_pPlayerState->test(PLAYERSTATE_URCHIN))
	{
	}
	// CutScene_UpgradeFork
	else if (m_pPlayerState->test(PLAYERSTATE_CUTSCENE_UPGRADEFORK))
	{
	}
	// Cheat
	else if (m_pPlayerState->test(PLAYERSTATE_CHEAT))
	{
	}
	// None
	else
	{
		if (strAnimState == L"None") return true;
		else if (strAnimState == L"Guard") return true;
		else if (strAnimState == L"Grapple_Aim") return true;
	}

	return false;
}

HRESULT CStateMachine_PlayerUpper::Ready_AnimState()
{
	if (FAILED(super::Add_AnimState(L"None", CPlayerState_None::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
		FAILED(super::Add_AnimState(L"Guard", CPlayerState_Guard::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
		FAILED(super::Add_AnimState(L"Grapple_Aim", CPlayerState_Grapple_Aim::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
		FAILED(super::Add_AnimState(L"Item_HeartKelp", CPlayerState_Item_HeartKelp::Create(m_pDevice, m_pContext, this, m_pPlayerState))))
		return E_FAIL;

	return S_OK;
}

CStateMachine_PlayerUpper* CStateMachine_PlayerUpper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine_PlayerUpper* pInstance = new CStateMachine_PlayerUpper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CStateMachine_PlayerUpper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CStateMachine_PlayerUpper::Clone(void* pArg)
{
	CStateMachine_PlayerUpper* pInstance = new CStateMachine_PlayerUpper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CStateMachine_PlayerUpper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine_PlayerUpper::Free()
{
	super::Free();
}