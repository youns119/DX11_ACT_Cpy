#include "pch.h"
#include "StateMachine_Player.h"

#include "GameObject.h"
#include "Model.h"
#include "Animation.h"

#include "Player.h"

#include "PlayerState_Idle.h"
#include "PlayerState_Walk.h"
#include "PlayerState_Run.h"
#include "PlayerState_Airbone.h"
#include "PlayerState_Swimming.h"
#include "PlayerState_Rising.h"
#include "PlayerState_LockOn.h"
#include "PlayerState_JumpLaunch.h"
#include "PlayerState_JumpLand.h"
#include "PlayerState_BackFlip.h"
#include "PlayerState_Evade.h"
#include "PlayerState_BackStep.h"
#include "PlayerState_NormalAttack.h"
#include "PlayerState_ChargeAttack.h"
#include "PlayerState_DashAttack.h"
#include "PlayerState_SwipeAttack.h"
#include "PlayerState_PlungeAttack.h"
#include "PlayerState_ParryAttack.h"
#include "PlayerState_BackStepAttack.h"
#include "PlayerState_Parry.h"
#include "PlayerState_ShellEquip.h"
#include "PlayerState_ShellEnter.h"
#include "PlayerState_ShellIdle.h"
#include "PlayerState_ShellCrawl.h"
#include "PlayerState_ShellParry.h"
#include "PlayerState_ShellAttacked.h"
#include "PlayerState_ShellSkill_RollOut.h"
#include "PlayerState_ShellSkill_RollOut_BackFlip.h"
#include "PlayerState_ShellSkill_Fizzle.h"
#include "PlayerState_Grapple_Throw.h"
#include "PlayerState_Grapple_Loop.h"
#include "PlayerState_Grabbed.h"
#include "PlayerState_NormalAttacked.h"
#include "PlayerState_HeavyAttacked_Front.h"
#include "PlayerState_HeavyAttacked_Back.h"
#include "PlayerState_SuperAttacked.h"
#include "PlayerState_GetUp_Front.h"
#include "PlayerState_GetUp_Back.h"
#include "PlayerState_NormalDeath.h"
#include "PlayerState_HeavyDeath_Front.h"
#include "PlayerState_HeavyDeath_Back.h"
#include "PlayerState_Hammer_Pick.h"
#include "PlayerState_Grapple_AttackStart.h"
#include "PlayerState_Grapple_Attack.h"
#include "PlayerState_Grapple_AttackEnd.h"
#include "PlayerState_Grapple_Shell.h"
#include "PlayerState_FinishAttack.h"
#include "PlayerState_MantisPunch.h"
#include "PlayerState_Urchin.h"
#include "PlayerState_CutScene_UpgradeFork.h"
#include "PlayerState_CutScene_MoonShellEnter.h"
#include "PlayerState_CutScene_MoonShellExit.h"
#include "PlayerState_CutScene_GrabFork.h"
#include "PlayerState_CutScene_Hide.h"
#include "PlayerState_CutScene_Talk.h"
#include "PlayerState_SuperUltraCheatMode.h"

CStateMachine_Player::CStateMachine_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CStateMachine_Player::CStateMachine_Player(const CStateMachine_Player& Prototype)
    : super(Prototype)
{
}

HRESULT CStateMachine_Player::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CStateMachine_Player::Initialize(void* pArg)
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

void CStateMachine_Player::Update(_float fTimeDelta)
{
    super::Update(fTimeDelta);
}

HRESULT CStateMachine_Player::ChangeState(const _wstring& strState)
{
    _bool bCanChange = Check_AnimGraph(strState);

    if (bCanChange)
        if (FAILED(super::ChangeState(strState)))
            return E_FAIL;

    return S_OK;
}

_bool CStateMachine_Player::Check_AnimGraph(const _wstring& strAnimState)
{
    _bool bTransition = m_pPlayer->Get_PlayerStat()->bTransition;

    if (strAnimState == L"CutScene_UpgradeFork" ||
        strAnimState == L"CutScene_MoonShellEnter" ||
        strAnimState == L"CutScene_MoonShellExit" ||
        strAnimState == L"CutScene_GrabFork" ||
        strAnimState == L"CutScene_Hide" ||
        strAnimState == L"CutScene_Talk" ||
        strAnimState == L"SuperUltraCheatMode")
        return true;

	if (strAnimState == L"ParryAttack" ||
		strAnimState == L"BackStepAttack")
		return true;

    if (strAnimState == L"Grabbed")
    {
		if (m_pPlayer->Get_PlayerStat()->bDeath)
			return false;
		else if (m_pPlayerState->test(PLAYERSTATE_GRABBED))
			return false;
        else return true;
    }

    if (m_pPlayerState->test(PLAYERSTATE_DEBUFF_FEAR))
    {
        if (strAnimState == L"Idle") return true;
        else if (strAnimState == L"Walk") return true;
        else if (strAnimState == L"Evade") return true;
        else if (strAnimState == L"ShellEnter") return true;
        else if (strAnimState == L"ShellIdle") return true;
        else if (strAnimState == L"ShellCrawl") return true;
        else if (strAnimState == L"ShellAttacked") return true;
        else if (strAnimState == L"NormalAttacked") return true;
        else if (strAnimState == L"HeavyAttacked_Front") return true;
        else if (strAnimState == L"HeavyAttacked_Back") return true;
        else if (strAnimState == L"SuperAttacked") return true;
        else if (strAnimState == L"GetUp_Front") return true;
        else if (strAnimState == L"GetUp_Back") return true;
        else if (strAnimState == L"NormalDeath") return true;
        else if (strAnimState == L"HeavyDeath_Front") return true;
        else if (strAnimState == L"HeavyDeath_Back") return true;
        else return false;
    }
    else if (m_pPlayerState->test(PLAYERSTATE_ITEM_HEARTKELP))
    {
        if (strAnimState == L"Idle") return true;
        else if (strAnimState == L"Walk") return true;
        else if (strAnimState == L"LockOn") return true;
    }

	// Idle
	if (m_pPlayerState->test(PLAYERSTATE_IDLE))
	{
		if (strAnimState == L"Walk") return true;
		else if (strAnimState == L"Run") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ChargeAttack") return true;
		else if (strAnimState == L"Parry") return true;
		else if (strAnimState == L"ShellEquip") return true;
		else if (strAnimState == L"ShellEnter") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
		else if (strAnimState == L"HammerChange") return true;
		else if (strAnimState == L"CutScene_UpgradeFork") return true;
	}
	// Walk
	else if (m_pPlayerState->test(PLAYERSTATE_WALK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Run") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ChargeAttack") return true;
		else if (strAnimState == L"Parry") return true;
		else if (strAnimState == L"ShellEquip") return true;
		else if (strAnimState == L"ShellEnter") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"HammerChange") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Run
	else if (m_pPlayerState->test(PLAYERSTATE_RUN))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Walk") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"DashAttack") return true;
		else if (strAnimState == L"Parry") return true;
		else if (strAnimState == L"ShellEquip") return true;
		else if (strAnimState == L"ShellEnter") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"HammerChange") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Airbone
	else if (m_pPlayerState->test(PLAYERSTATE_AIRBONE))
	{
		if (strAnimState == L"Swimming") return true;
		else if (strAnimState == L"JumpLand") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"SwipeAttack") return true;
		else if (strAnimState == L"Parry") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Swimming
	else if (m_pPlayerState->test(PLAYERSTATE_SWIMMING))
	{
		if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLand") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"SwipeAttack") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Rising
	else if (m_pPlayerState->test(PLAYERSTATE_RISING))
	{
		if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"Swimming") return true;
		else if (strAnimState == L"JumpLand") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"SwipeAttack") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// LockOn
	else if (m_pPlayerState->test(PLAYERSTATE_LOCKON))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Walk") return true;
		else if (strAnimState == L"Run") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ChargeAttack") return true;
		else if (strAnimState == L"Parry") return true;
		else if (strAnimState == L"ShellEquip") return true;
		else if (strAnimState == L"ShellEnter") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"HammerChange") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// JumpLaunch
	else if (m_pPlayerState->test(PLAYERSTATE_JUMPLAUNCH))
	{
		if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"SwipeAttack") return true;
		else if (strAnimState == L"Parry") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// JumpLand
	else if (m_pPlayerState->test(PLAYERSTATE_JUMPLAND))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ChargeAttack") return true;
		else if (strAnimState == L"Parry") return true;
		else if (strAnimState == L"ShellEquip") return true;
		else if (strAnimState == L"ShellEnter") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// BackFlip
	else if (m_pPlayerState->test(PLAYERSTATE_BACKFLIP))
	{
		if (strAnimState == L"Swimming") return true;
		else if (strAnimState == L"Rising") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"SwipeAttack") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Evade
	else if (m_pPlayerState->test(PLAYERSTATE_EVADE))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack" && bTransition) return true;
		else if (strAnimState == L"ChargeAttack" && bTransition) return true;
		else if (strAnimState == L"ShellEquip" && bTransition) return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_RollOut" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_Fizzle" && bTransition) return true;
		else if (strAnimState == L"FinishAttack" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// BackStep
	else if (m_pPlayerState->test(PLAYERSTATE_BACKSTEP))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"NormalAttack" && bTransition) return true;
		else if (strAnimState == L"ChargeAttack" && bTransition) return true;
		else if (strAnimState == L"BackStepAttack") return true;
		else if (strAnimState == L"ShellEquip" && bTransition) return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_RollOut" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_Fizzle" && bTransition) return true;
		else if (strAnimState == L"FinishAttack" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// NormalAttack
	else if (m_pPlayerState->test(PLAYERSTATE_NORMALATTACK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_RollOut" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_Fizzle" && bTransition) return true;
		else if (strAnimState == L"FinishAttack" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ChargeAttack
	else if (m_pPlayerState->test(PLAYERSTATE_CHARGEATTACK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_RollOut" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_Fizzle" && bTransition) return true;
		else if (strAnimState == L"FinishAttack" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
		else if (strAnimState == L"HammerPick") return true;
	}
	// DashAttack
	else if (m_pPlayerState->test(PLAYERSTATE_DASHATTACK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_RollOut" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_Fizzle" && bTransition) return true;
		else if (strAnimState == L"FinishAttack" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
		else if (strAnimState == L"HammerPick") return true;
	}
	// SwipeAttack
	else if (m_pPlayerState->test(PLAYERSTATE_SWIPEATTACK))
	{
		if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"PlungeAttack") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// PlungeAttack
	else if (m_pPlayerState->test(PLAYERSTATE_PLUNGEATTACK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_RollOut" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_Fizzle" && bTransition) return true;
		else if (strAnimState == L"FinishAttack" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ParryAttack
	else if (m_pPlayerState->test(PLAYERSTATE_PARRYATTACK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_RollOut" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_Fizzle" && bTransition) return true;
		else if (strAnimState == L"FinishAttack" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// BackStepAttack
	else if (m_pPlayerState->test(PLAYERSTATE_BACKSTEPATTACK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_RollOut" && bTransition) return true;
		else if (strAnimState == L"ShellSkill_Fizzle" && bTransition) return true;
		else if (strAnimState == L"FinishAttack" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch" && bTransition) return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Parry
	else if (m_pPlayerState->test(PLAYERSTATE_PARRY))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"Swimming") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"ParryAttack") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellEquip
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLEQUIP))
	{
		if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"NormalAttack" && bTransition) return true;
		else if (strAnimState == L"ChargeAttack" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellEnter
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLENTER))
	{
		if (strAnimState == L"ShellIdle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ChargeAttack") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellIdle
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLIDLE))
	{
		if (strAnimState == L"ShellCrawl") return true;
		else if (strAnimState == L"ShellParry") return true;
		else if (strAnimState == L"ShellAttacked") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ChargeAttack") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellCrawl
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLCRAWL))
	{
		if (strAnimState == L"ShellIdle") return true;
		else if (strAnimState == L"ShellParry") return true;
		else if (strAnimState == L"ShellAttacked") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ChargeAttack") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellParry
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLPARRY))
	{
		if (strAnimState == L"ShellEnter") return true;
		else if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLaunch") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade") return true;
		else if (strAnimState == L"BackStep") return true;
		else if (strAnimState == L"NormalAttack") return true;
		else if (strAnimState == L"ChargeAttack") return true;
		else if (strAnimState == L"ParryAttack") return true;
		else if (strAnimState == L"ShellSkill_RollOut") return true;
		else if (strAnimState == L"ShellSkill_Fizzle") return true;
		else if (strAnimState == L"FinishAttack") return true;
		else if (strAnimState == L"ActiveSkill_MantisPunch") return true;
		else if (strAnimState == L"ActiveSkill_Urchin") return true;
		else if (strAnimState == L"Grapple_Throw") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellAttacked
	else if(m_pPlayerState->test(PLAYERSTATE_SHELLATTACKED))
	{
		if (strAnimState == L"ShellIdle") return true;
		else if (strAnimState == L"ShellParry") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellSkill_RollOut
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLSKILL_ROLLOUT))
	{
		if (strAnimState == L"ShellParry") return true;
		else if (strAnimState == L"ShellSkill_RollOut_BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellSkill_RollOut_BackFlip
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLSKILL_ROLLOUT_BACKFLIP))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ShellSkill_Fizzle
	else if (m_pPlayerState->test(PLAYERSTATE_SHELLSKILL_FIZZLE))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Grapple_Throw
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_THROW))
	{
		if (strAnimState == L"Idle" && bTransition) return true;
		else if (strAnimState == L"Airbone" && bTransition) return true;
		else if (strAnimState == L"LockOn" && bTransition) return true;
		else if (strAnimState == L"Grapple_Loop") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
		else if (strAnimState == L"Grapple_AttackStart") return true;
		else if (strAnimState == L"Grapple_Shell") return true;
	}
	// Grapple_Loop
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_LOOP))
	{
		if (strAnimState == L"Rising") return true;
		else if (strAnimState == L"Grapple_AttackEnd") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Grabbed
	else if (m_pPlayerState->test(PLAYERSTATE_GRABBED))
	{
		if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// NormalAttacked
	else if (m_pPlayerState->test(PLAYERSTATE_NORMALATTACKED))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
		else if (strAnimState == L"NormalDeath") return true;
	}
	// HeavyAttacked_Front
	else if (m_pPlayerState->test(PLAYERSTATE_HEAVYATTACKED_FRONT))
	{
		if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"GetUp_Front") return true;
		else if (strAnimState == L"HeavyDeath_Front") return true;
	}
	// HeavyAttacked_Back
	else if (m_pPlayerState->test(PLAYERSTATE_HEAVYATTACKED_BACK))
	{
		if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"GetUp_Back") return true;
		else if (strAnimState == L"HeavyDeath_Back") return true;
	}
	// SuperAttacked
	else if (m_pPlayerState->test(PLAYERSTATE_SUPERATTACKED))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"NormalDeath") return true;
	}
	// GetUp_Front
	else if (m_pPlayerState->test(PLAYERSTATE_GETUP_FRONT))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// GetUp_Back
	else if (m_pPlayerState->test(PLAYERSTATE_GETUP_BACK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// NormalDeath
	else if (m_pPlayerState->test(PLAYERSTATE_NORMALDEATH))
	{
		if (strAnimState == L"CutScene_MoonShellExit") return true;
		else if (strAnimState == L"Idle") return true; // 리스폰 디버깅용
	}
	// HeavyDeath_Front
	else if (m_pPlayerState->test(PLAYERSTATE_HEAVYDEATH_FRONT))
	{
		if (strAnimState == L"CutScene_MoonShellExit") return true;
		else if (strAnimState == L"Idle") return true; // 리스폰 디버깅용
	}
	// HeavyDeath_Back
	else if (m_pPlayerState->test(PLAYERSTATE_HEAVYDEATH_BACK))
	{
		if (strAnimState == L"CutScene_MoonShellExit") return true;
		else if (strAnimState == L"Idle") return true; // 리스폰 디버깅용
	}
	// HammerPick
	else if (m_pPlayerState->test(PLAYERSTATE_HAMMER_PICK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"BackFlip") return true;
		else if (strAnimState == L"LockOn") return true;
		else if (strAnimState == L"JumpLaunch" && bTransition) return true;
		else if (strAnimState == L"Evade" && bTransition) return true;
		else if (strAnimState == L"BackStep" && bTransition) return true;
		else if (strAnimState == L"ShellEnter" && bTransition) return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Grapple_AttackStart
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_ATTACKSTART))
	{
		if (strAnimState == L"Grapple_Attack") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Grapple_Attack
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_ATTACK))
	{
		if (strAnimState == L"Grapple_AttackEnd") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Grapple_AttackEnd
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_ATTACKEND))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// Grapple_Shell
	else if (m_pPlayerState->test(PLAYERSTATE_GRAPPLE_SHELL))
	{
		if (strAnimState == L"ShellEquip") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// FinishAttack
	else if (m_pPlayerState->test(PLAYERSTATE_FINISHATTACK))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		else if (strAnimState == L"NormalAttacked") return true;
		else if (strAnimState == L"HeavyAttacked_Front") return true;
		else if (strAnimState == L"HeavyAttacked_Back") return true;
		else if (strAnimState == L"SuperAttacked") return true;
	}
	// ActiveSkill_MantisPunch
	else if (m_pPlayerState->test(PLAYERSTATE_MANTISPUNCH))
	{
		if (strAnimState == L"Idle") return true;
		//else if (strAnimState == L"NormalAttacked") return true;
		//else if (strAnimState == L"HeavyAttacked_Front") return true;
		//else if (strAnimState == L"HeavyAttacked_Back") return true;
		//else if (strAnimState == L"SuperAttacked") return true;
	}
	// ActiveSkill_Urchin
	else if (m_pPlayerState->test(PLAYERSTATE_URCHIN))
	{
		if (strAnimState == L"Idle") return true;
		else if (strAnimState == L"Airbone") return true;
		//else if (strAnimState == L"NormalAttacked") return true;
		//else if (strAnimState == L"HeavyAttacked_Front") return true;
		//else if (strAnimState == L"HeavyAttacked_Back") return true;
		//else if (strAnimState == L"SuperAttacked") return true;
	}
	// CutScene_UpgradeFork
	if (m_pPlayerState->test(PLAYERSTATE_CUTSCENE_UPGRADEFORK))
	{
		if (strAnimState == L"Idle") return true;
	}
	// CutScene_MoonShellEnter
	if (m_pPlayerState->test(PLAYERSTATE_CUTSCENE_MOONSHELLENTER))
	{
		if (strAnimState == L"CutScene_MoonShellExit") return true;
	}
	// CutScene_MoonShellExit
	if (m_pPlayerState->test(PLAYERSTATE_CUTSCENE_MOONSHELLEXIT))
	{
		if (strAnimState == L"Idle") return true;
	}
	// CutScene_GrabFork
	if (m_pPlayerState->test(PLAYERSTATE_CUTSCENE_GRABFORK))
	{
		if (strAnimState == L"Idle") return true;
	}
	// CutScene_Hide
	if (m_pPlayerState->test(PLAYERSTATE_CUTSCENE_HIDE))
	{
		if (strAnimState == L"ShellParry") return true;
	}
	// CutScene_Hide
	if (m_pPlayerState->test(PLAYERSTATE_CUTSCENE_TALK))
	{
		if (strAnimState == L"Idle") return true;
	}
	// Cheat
	else if (m_pPlayerState->test(PLAYERSTATE_CHEAT))
	{
		if (strAnimState == L"Idle") return true;
	}

    return false;
}

HRESULT CStateMachine_Player::Ready_AnimState()
{
    if (FAILED(super::Add_AnimState(L"Idle", CPlayerState_Idle::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Walk", CPlayerState_Walk::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Run", CPlayerState_Run::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Airbone", CPlayerState_Airbone::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Swimming", CPlayerState_Swimming::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Rising", CPlayerState_Rising::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"LockOn", CPlayerState_LockOn::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"JumpLaunch", CPlayerState_JumpLaunch::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"JumpLand", CPlayerState_JumpLand::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"BackFlip", CPlayerState_BackFlip::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Evade", CPlayerState_Evade::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"BackStep", CPlayerState_BackStep::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"NormalAttack", CPlayerState_NormalAttack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ChargeAttack", CPlayerState_ChargeAttack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"DashAttack", CPlayerState_DashAttack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"SwipeAttack", CPlayerState_SwipeAttack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"PlungeAttack", CPlayerState_PlungeAttack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ParryAttack", CPlayerState_ParryAttack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"BackStepAttack", CPlayerState_BackStepAttack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Parry", CPlayerState_Parry::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellEquip", CPlayerState_ShellEquip::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellEnter", CPlayerState_ShellEnter::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellIdle", CPlayerState_ShellIdle::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellCrawl", CPlayerState_ShellCrawl::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellParry", CPlayerState_ShellParry::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellAttacked", CPlayerState_ShellAttacked::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellSkill_RollOut", CPlayerState_ShellSkill_RollOut::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellSkill_RollOut_BackFlip", CPlayerState_ShellSkill_RollOut_BackFlip::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ShellSkill_Fizzle", CPlayerState_ShellSkill_Fizzle::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Grapple_Throw", CPlayerState_Grapple_Throw::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Grapple_Loop", CPlayerState_Grapple_Loop::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Grabbed", CPlayerState_Grabbed::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"NormalAttacked", CPlayerState_NormalAttacked::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"HeavyAttacked_Front", CPlayerState_HeavyAttacked_Front::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"HeavyAttacked_Back", CPlayerState_HeavyAttacked_Back::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"SuperAttacked", CPlayerState_SuperAttacked::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"GetUp_Front", CPlayerState_GetUp_Front::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"GetUp_Back", CPlayerState_GetUp_Back::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"NormalDeath", CPlayerState_NormalDeath::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"HeavyDeath_Front", CPlayerState_HeavyDeath_Front::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"HeavyDeath_Back", CPlayerState_HeavyDeath_Back::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"HammerPick", CPlayerState_Hammer_Pick::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Grapple_AttackStart", CPlayerState_Grapple_AttackStart::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Grapple_Attack", CPlayerState_Grapple_Attack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Grapple_AttackEnd", CPlayerState_Grapple_AttackEnd::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"Grapple_Shell", CPlayerState_Grapple_Shell::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"FinishAttack", CPlayerState_FinishAttack::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ActiveSkill_MantisPunch", CPlayerState_MantisPunch::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"ActiveSkill_Urchin", CPlayerState_Urchin::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"CutScene_UpgradeFork", CPlayerState_CutScene_UpgradeFork::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"CutScene_MoonShellEnter", CPlayerState_CutScene_MoonShellEnter::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"CutScene_MoonShellExit", CPlayerState_CutScene_MoonShellExit::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"CutScene_GrabFork", CPlayerState_CutScene_GrabFork::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"CutScene_Hide", CPlayerState_CutScene_Hide::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"CutScene_Talk", CPlayerState_CutScene_Talk::Create(m_pDevice, m_pContext, this, m_pPlayerState))) ||
        FAILED(super::Add_AnimState(L"SuperUltraCheatMode", CPlayerState_SuperUltraCheatMode::Create(m_pDevice, m_pContext, this, m_pPlayerState))))
        return E_FAIL;

    return S_OK;
}

CStateMachine_Player* CStateMachine_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStateMachine_Player* pInstance = new CStateMachine_Player(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CStateMachine_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CStateMachine_Player::Clone(void* pArg)
{
    CStateMachine_Player* pInstance = new CStateMachine_Player(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Create : CStateMachine_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStateMachine_Player::Free()
{
    super::Free();
}