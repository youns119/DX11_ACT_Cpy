#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"

#include "ESkill.h"
#include "FSkill.h"

BEGIN(Client)

class CSkill_DataBase
{
private:
	CSkill_DataBase()
	{
		_uint iActionSize = static_cast<_uint>(ACTIONSKILL::ACTIONSKILL_END);
		_uint iActiveSize = static_cast<_uint>(ACTIVESKILL::ACTIVESKILL_END);

		m_vecActionSkill.resize(iActionSize);
		m_vecActiveSkill.resize(iActiveSize);

		m_vecActionSkill[static_cast<_uint>(ACTIONSKILL::ACTIONSKILL_TELEPORT)] =
		{ ACTIONSKILL::ACTIONSKILL_TELEPORT, false, L"ActionSkill_Teleport" };
		m_vecActionSkill[static_cast<_uint>(ACTIONSKILL::ACTIONSKILL_HAMMER)] =
		{ ACTIONSKILL::ACTIONSKILL_HAMMER, false, L"ActionSkill_Hammer"};
		m_vecActionSkill[static_cast<_uint>(ACTIONSKILL::ACTIONSKILL_FISHING)] =
		{ ACTIONSKILL::ACTIONSKILL_FISHING, false, L"ActionSkill_Fishing" };
		m_vecActionSkill[static_cast<_uint>(ACTIONSKILL::ACTIONSKILL_FINISH)] =
		{ ACTIONSKILL::ACTIONSKILL_FINISH, false, L"ActionSkill_Finish" };

		m_vecActiveSkill[static_cast<_uint>(ACTIVESKILL::ACTIVESKILL_MANTISPUNCH)] =
		{ ACTIVESKILL::ACTIVESKILL_MANTISPUNCH, false, 3.f, L"ActiveSkill_MantisPunch" };
		m_vecActiveSkill[static_cast<_uint>(ACTIVESKILL::ACTIVESKILL_URCHIN)] =
		{ ACTIVESKILL::ACTIVESKILL_URCHIN, false, 1.f, L"ActiveSkill_Urchin"};
		m_vecActiveSkill[static_cast<_uint>(ACTIVESKILL::ACTIVESKILL_BOBBITTRAP)] =
		{ ACTIVESKILL::ACTIVESKILL_BOBBITTRAP, false, 2.f, L"ActiveSkill_BobbitTrap" };
	}

public:
	static CSkill_DataBase& Get()
	{
		static CSkill_DataBase Instance;
		return Instance;
	}

	const FActionSkill& Get_ActionSkill(ACTIONSKILL eActionSkill) const
	{
		return m_vecActionSkill[static_cast<_uint>(eActionSkill)];
	}
	const FActiveSkill& Get_ActiveSkill(ACTIVESKILL eActiveSkill) const
	{
		return m_vecActiveSkill[static_cast<_uint>(eActiveSkill)];
	}

	const vector<FActionSkill>& Get_ActionSkills() const
	{
		return m_vecActionSkill;
	}
	const vector<FActiveSkill>& Get_ActiveSkills() const
	{
		return m_vecActiveSkill;
	}

public :
	void UnLock_ActionSkill(ACTIONSKILL eActionSkill)
	{
		m_vecActionSkill[static_cast<_uint>(eActionSkill)].bUnLocked = true;
	}
	void UnLock_ActiveSkill(ACTIVESKILL eActiveSkill)
	{
		m_vecActiveSkill[static_cast<_uint>(eActiveSkill)].bUnLocked = true;
	}

private:
	vector<FActionSkill> m_vecActionSkill;
	vector<FActiveSkill> m_vecActiveSkill;
};

END