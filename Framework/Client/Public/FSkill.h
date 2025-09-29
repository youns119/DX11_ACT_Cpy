#pragma once

BEGIN(Client)

struct FActionSkill
{
	ACTIONSKILL eActionSkill{ ACTIONSKILL::ACTIONSKILL_END };

	_bool bUnLocked{};

	_wstring strActionSkill{};
};

struct FActiveSkill
{
	ACTIVESKILL eActiveSkill{ ACTIVESKILL::ACTIVESKILL_END };

	_bool bUnLocked{};
	_float fSkillPoint{};

	_wstring strActiveSkill{};
};

END