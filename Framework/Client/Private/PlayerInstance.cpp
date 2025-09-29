#include "pch.h"
#include "PlayerInstance.h"

#include "GameInstance.h"
#include "Player.h"

CPlayerInstance::CPlayerInstance()
{
}

void CPlayerInstance::Save_PlayerData(const CPlayer& Player)
{
    const FPlayerStat* tPlayerStat = Player.Get_PlayerStat_Const();
    Save_PlayerStat(tPlayerStat);

    const FWeaponStat* tWeaponStat = Player.Get_PlayerWeaponStat_Const();
    const FShellStat* tShellStat = Player.Get_PlayerWeaponShellStat_Const();
    if (tWeaponStat != nullptr)
        Save_WeaponStat(tWeaponStat, tShellStat);

    // Shell
    if (m_tPlayerStat.bHasShell)
    {
        const FShellStat* tShellStat = Player.Get_PlayerShellStat_Const();
        Save_ShellStat(tShellStat);
    }
    else
    {
        m_ePlayerShell = SHELL::SHELL_END;
        m_fPlayerShellHP = 0.f;
    }
}

void CPlayerInstance::Load_PlayerData(CPlayer& Player)
{
    // Player
    Player.Get_PlayerStat()->iLevel = m_tPlayerStat.iLevel;
    Player.Get_PlayerStat()->iVitality = m_tPlayerStat.iVitality;
    Player.Get_PlayerStat()->iResistance = m_tPlayerStat.iResistance;
    Player.Get_PlayerStat()->iAttackPower = m_tPlayerStat.iAttackPower;
    Player.Get_PlayerStat()->iMSG = m_tPlayerStat.iMSG;

    Player.Get_PlayerStat()->fMaxHP = m_tPlayerStat.fMaxHP;
    Player.Get_PlayerStat()->fCurrHP = m_tPlayerStat.fCurrHP;

    Player.Get_PlayerStat()->fMaxSkillPoint = m_tPlayerStat.fMaxSkillPoint;
    Player.Get_PlayerStat()->fCurrSkillPoint = m_tPlayerStat.fCurrSkillPoint;

    Player.Get_PlayerStat()->iHookCount = m_tPlayerStat.iHookCount;
    Player.Get_PlayerStat()->iHeartKelpCount = m_tPlayerStat.iHeartKelpCount;

    Player.Get_PlayerStat()->bTeleport = m_tPlayerStat.bTeleport;

    Player.Get_PlayerStat()->iShellLevel = m_tPlayerStat.iShellLevel;
    Player.Get_PlayerStat()->iShellNum = m_tPlayerStat.iShellNum;
    Player.Get_PlayerStat()->vShellPos = m_tPlayerStat.vShellPos;
    Player.Get_PlayerStat()->vShellLook = m_tPlayerStat.vShellLook;
    Player.Get_PlayerStat()->vShellRight = m_tPlayerStat.vShellRight;

    Player.Get_PlayerStat()->ePrimaryActiveSkill = m_tPlayerStat.ePrimaryActiveSkill;
    Player.Get_PlayerStat()->eSecondaryActiveSkill = m_tPlayerStat.eSecondaryActiveSkill;
}

void CPlayerInstance::Load_EquipData(CPlayer& Player)
{
    // Weapon
    Player.Equip_Weapon(m_tWeapon.iLevel, m_tWeapon.fDamage, m_eWeaponShell, m_fWeaponShellHP);

    // Shell
    if (m_tPlayerStat.bHasShell)
        Player.Equip_Shell_NoSound(m_ePlayerShell, m_fPlayerShellHP);
}

void CPlayerInstance::Save_Transition(_fmatrix matTransition)
{
    m_bTransition = true;
    m_matTransition = matTransition;
}

void CPlayerInstance::Load_Transition(_float4x4& matTransition)
{
    if (m_bTransition)
    {
        m_bTransition = false;
        XMStoreFloat4x4(&matTransition, m_matTransition);
    }
}

void CPlayerInstance::Initial_PlayerStat(FPlayerStat* tPlayerStat)
{
    if (tPlayerStat != nullptr)
        Save_PlayerStat(tPlayerStat);
}

void CPlayerInstance::Initial_WeaponStat(FWeaponStat* tWeaponStat, FShellStat* tShellData)
{
    if (tWeaponStat != nullptr)
        Save_WeaponStat(tWeaponStat, tShellData);
}

void CPlayerInstance::Initial_ShellStat(FShellStat* tShellStat)
{
    if (tShellStat != nullptr)
    {
        m_tPlayerStat.bHasShell = true;
        Save_ShellStat(tShellStat);
    }
    else m_tPlayerStat.bHasShell = false;
}

void CPlayerInstance::Save_PlayerStat(const FPlayerStat* tPlayerStat)
{
    m_tPlayerStat.iLevel = tPlayerStat->iLevel;
    m_tPlayerStat.iVitality = tPlayerStat->iVitality;
    m_tPlayerStat.iResistance = tPlayerStat->iResistance;
    m_tPlayerStat.iAttackPower = tPlayerStat->iAttackPower;
    m_tPlayerStat.iMSG = tPlayerStat->iMSG;

    m_tPlayerStat.fMaxHP = tPlayerStat->fMaxHP;
    m_tPlayerStat.fCurrHP = tPlayerStat->fCurrHP;

    m_tPlayerStat.fMaxSkillPoint = tPlayerStat->fMaxSkillPoint;
    m_tPlayerStat.fCurrSkillPoint = tPlayerStat->fCurrSkillPoint;

    m_tPlayerStat.iHookCount = tPlayerStat->iHookCount;
    m_tPlayerStat.iHeartKelpCount = tPlayerStat->iHeartKelpCount;

    m_tPlayerStat.bHasShell = tPlayerStat->bHasShell;

    m_tPlayerStat.bTeleport = tPlayerStat->bTeleport;

    m_tPlayerStat.iShellLevel = tPlayerStat->iShellLevel;
    m_tPlayerStat.iShellNum = tPlayerStat->iShellNum;
    m_tPlayerStat.vShellPos = tPlayerStat->vShellPos;
    m_tPlayerStat.vShellLook = tPlayerStat->vShellLook;
    m_tPlayerStat.vShellRight = tPlayerStat->vShellRight;

    m_tPlayerStat.ePrimaryActiveSkill = tPlayerStat->ePrimaryActiveSkill;
    m_tPlayerStat.eSecondaryActiveSkill = tPlayerStat->eSecondaryActiveSkill;
}

void CPlayerInstance::Save_WeaponStat(const FWeaponStat* tWeaponStat, const FShellStat* tShellStat)
{
    m_tWeapon.iLevel = tWeaponStat->iLevel;
    m_tWeapon.fDamage = tWeaponStat->fDamage;

    if (tWeaponStat->bHasShell && tShellStat != nullptr)
    {
        m_eWeaponShell = tShellStat->eShell;
        m_fWeaponShellHP = tShellStat->fShellCurrHP;
    }
    else
    {
        m_eWeaponShell = SHELL::SHELL_END;
        m_fWeaponShellHP = 0.f;
    }
}

void CPlayerInstance::Save_ShellStat(const FShellStat* tShellStat)
{
    m_ePlayerShell = tShellStat->eShell;
    m_fPlayerShellHP = tShellStat->fShellCurrHP;
}

CPlayerInstance* CPlayerInstance::Create()
{
    return new CPlayerInstance;
}

void CPlayerInstance::Free()
{
}