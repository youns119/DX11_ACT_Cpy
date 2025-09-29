#pragma once

#include "Client_Defines.h"

#include "IPlayerInstance.h"

BEGIN(Client)

class CPlayerManager final
{
private :
	CPlayerManager();
	virtual ~CPlayerManager() = default;

public :
	static void Init(IPlayerInstance* pPlayerInstance);

public :
	static IPlayerInstance* Get_PlayerInstance() { return m_pPlayerInstance; }

public :
	static void Save_PlayerData(const CPlayer& Player);
	static void Load_PlayerData(CPlayer& Player);
	static void Load_EquipData(CPlayer& Player);

public:
	static void Save_Transition(_fmatrix matTransition);
	static void Load_Transition(_float4x4& matTransition);

public :
	static void Initial_PlayerStat(FPlayerStat* tPlayerStat);
	static void Initial_WeaponStat(FWeaponStat* tWeaponStat = nullptr, FShellStat* tShellData = nullptr);
	static void Initial_ShellStat(FShellStat* tShellStat);

public :
	static void Release();

private :
	static IPlayerInstance* m_pPlayerInstance;
};

#define PLAYERINST CPlayerManager::Get_PlayerInstance()

END