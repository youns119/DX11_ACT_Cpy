#pragma once

#include "Engine_Defines.h"

#include "FPlayerStat.h"
#include "FShell.h"
#include "FWeapon.h"

BEGIN(Client)

class CPlayer;

class IPlayerInstance abstract
{
public :
	virtual ~IPlayerInstance() = default;

public :
	virtual void Save_PlayerData(const CPlayer& Player) PURE;
	virtual void Load_PlayerData(CPlayer& Player) PURE;
	virtual void Load_EquipData(CPlayer& Player) PURE;

public :
	virtual void Save_Transition(_fmatrix matTransition) PURE;
	virtual void Load_Transition(_float4x4& matTransition) PURE;

public :
	virtual void Initial_PlayerStat(FPlayerStat* tPlayerStat) PURE;
	virtual void Initial_WeaponStat(FWeaponStat* tWeaponStat = nullptr, FShellStat* tShellData = nullptr) PURE;
	virtual void Initial_ShellStat(FShellStat* tShellStat) PURE;

public :
	virtual void Free() PURE;
};

END