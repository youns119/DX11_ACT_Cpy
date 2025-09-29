#pragma once

BEGIN(Client)

struct FWeaponStat
{
	_uint iLevel{ 1 };
	_uint iMaxLevel{ 6 };

	_float fDamage{ 10.f };

	_bool bHasShell{};
};

END