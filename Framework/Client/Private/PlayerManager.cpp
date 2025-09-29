#include "pch.h"
#include "PlayerManager.h"

IPlayerInstance* CPlayerManager::m_pPlayerInstance = nullptr;

CPlayerManager::CPlayerManager()
{
}

void CPlayerManager::Init(IPlayerInstance* pPlayerInstance)
{
	m_pPlayerInstance = pPlayerInstance;
}

void CPlayerManager::Save_PlayerData(const CPlayer& Player)
{
	m_pPlayerInstance->Save_PlayerData(Player);
}

void CPlayerManager::Load_PlayerData(CPlayer& Player)
{
	m_pPlayerInstance->Load_PlayerData(Player);
}

void CPlayerManager::Load_EquipData(CPlayer& Player)
{
	m_pPlayerInstance->Load_EquipData(Player);
}

void CPlayerManager::Save_Transition(_fmatrix matTransition)
{
	m_pPlayerInstance->Save_Transition(matTransition);
}

void CPlayerManager::Load_Transition(_float4x4& matTransition)
{
	m_pPlayerInstance->Load_Transition(matTransition);
}

void CPlayerManager::Initial_PlayerStat(FPlayerStat* tPlayerStat)
{
	m_pPlayerInstance->Initial_PlayerStat(tPlayerStat);
}

void CPlayerManager::Initial_WeaponStat(FWeaponStat* tWeaponStat, FShellStat* tShellData)
{
	m_pPlayerInstance->Initial_WeaponStat(tWeaponStat, tShellData);
}

void CPlayerManager::Initial_ShellStat(FShellStat* tShellStat)
{
	m_pPlayerInstance->Initial_ShellStat(tShellStat);
}

void CPlayerManager::Release()
{
	m_pPlayerInstance->Free();
	Safe_Delete(m_pPlayerInstance);
}