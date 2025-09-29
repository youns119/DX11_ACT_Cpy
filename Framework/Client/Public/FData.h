#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Client)

struct FDialogueData
{
	_int	iStateNum{};
	_tchar	szSpeaker[MAX_PATH];
	_uint	iFaceTextureNum{};
	_tchar	szDialogue[MAX_PATH] = {};
};

struct FItemData
{
	_uint iItemNum{};
	_uint iPrice = {};
	_tchar szName[MAX_PATH] = {};
	_tchar szDescription[MAX_PATH] = {};
};

struct FSkillData
{
	_uint iSkillNum{};
	_tchar szSkillName[MAX_PATH] = {};
	_tchar szSkillDescription[MAX_PATH] = {};
};

struct FSheleportData
{
	_uint iSheleportNum = {};
	_tchar szLocation[MAX_PATH] = {};
	_tchar szThemeFilePath[MAX_PATH] = {};
	_tchar szName[MAX_PATH] = {};
};

struct FShellDescData
{
	_uint iShellNum{};
	_float4 vTextColor{};
	_tchar szShellName[MAX_PATH] = {};
	_tchar szSkillName[MAX_PATH] = {};
	_tchar szSkillDescription[MAX_PATH] = {};
};
END