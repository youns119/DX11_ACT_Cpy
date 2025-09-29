#pragma once

#include "Client_Defines.h"

#include "IPlayerInstance.h"

BEGIN(Client)

class CPlayer;

class CPlayerInstance final
	: public IPlayerInstance
{
private :
	CPlayerInstance();
	virtual ~CPlayerInstance() = default;

public :
	virtual void Save_PlayerData(const CPlayer& Player) override;
	virtual void Load_PlayerData(CPlayer& Player) override;
	virtual void Load_EquipData(CPlayer& Player) override;

public :
	virtual void Save_Transition(_fmatrix matTransition) override;
	virtual void Load_Transition(_float4x4& matTransition) override;

public:
	virtual void Initial_PlayerStat(FPlayerStat* tPlayerStat) override;
	virtual void Initial_WeaponStat(FWeaponStat* tWeaponStat = nullptr, FShellStat* tShellData = nullptr) override;
	virtual void Initial_ShellStat(FShellStat* tShellStat) override;

private :
	FPlayerStat m_tPlayerStat{};
	SHELL m_ePlayerShell{ SHELL::SHELL_END };
	_float m_fPlayerShellHP{};

	FWeaponStat m_tWeapon{};
	SHELL m_eWeaponShell{ SHELL::SHELL_END };
	_float m_fWeaponShellHP{};

private :
	_bool m_bTransition{};
	_matrix m_matTransition{};

private :
	void Save_PlayerStat(const FPlayerStat* tPlayerStat);
	void Save_WeaponStat(const FWeaponStat* tWeaponStat, const FShellStat* tShellStat);
	void Save_ShellStat(const FShellStat* tShellStat);

public :
	static CPlayerInstance* Create();
	virtual void Free() override;
};

END