#pragma once

BEGIN(Client)

struct FShellStat
{
	SHELL eShell{ SHELL::SHELL_END };
	SHELLWEIGHT eShellWeight{ SHELLWEIGHT::SHELLWEIGHT_END };

	_bool bMagnet{};
	_float fShellMaxHP{};
	_float fShellCurrHP{};
	_float fShellDefense{};

	_wstring strSkillName{};
	_float fSkillPoint{};
};

struct FShellData
{
	_string strFilePath{};
	_wstring strModel{};
	_wstring strSkill{};

	_float fColliderRadius{};
	_float3 vColliderCenter{};
};

END