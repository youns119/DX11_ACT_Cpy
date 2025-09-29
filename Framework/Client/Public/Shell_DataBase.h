#pragma once

#include "EShell.h"
#include "FShell.h"

BEGIN(Client)

class CShell_DataBase
{
private :
	CShell_DataBase()
	{
		_uint iSize = static_cast<_uint>(SHELL::SHELL_END);

		m_vecShellStat.resize(iSize);
		m_vecShellData.resize(iSize);

		m_vecShellStat[static_cast<_uint>(SHELL::SHELL_HOME)] =
		{ SHELL::SHELL_HOME, SHELLWEIGHT::SHELLWEIGHT_M, false, 100.f, 100.f, 50.f, L"None", 0.f };
		m_vecShellStat[static_cast<_uint>(SHELL::SHELL_SODACAN)] =
		{ SHELL::SHELL_SODACAN, SHELLWEIGHT::SHELLWEIGHT_M, true, 50.f, 50.f, 30.f, L"ShellSkill_Fizzle", 1.f };
		m_vecShellStat[static_cast<_uint>(SHELL::SHELL_MIRRORBALL)] = 
		{ SHELL::SHELL_MIRRORBALL, SHELLWEIGHT::SHELLWEIGHT_L, true, 200.f, 200.f, 25.f, L"ShellSkill_RollOut", 1.f };
		m_vecShellStat[static_cast<_uint>(SHELL::SHELL_DUCK)] =
		{ SHELL::SHELL_DUCK, SHELLWEIGHT::SHELLWEIGHT_S, false, 80.f, 80.f, 35.f, L"ShellSkill_Fizzle", 1.f };
		m_vecShellStat[static_cast<_uint>(SHELL::SHELL_TENNISBALL)] =
		{ SHELL::SHELL_TENNISBALL, SHELLWEIGHT::SHELLWEIGHT_M, false, 80.f, 80.f, 35.f, L"ShellSkill_RollOut", 1.f };

		m_vecShellData[static_cast<_uint>(SHELL::SHELL_HOME)] =
		{ "../Bin/DataFiles/PartObjectData/Shell_Home", L"Prototype_Component_Model_Shell_Home", L"None", 1.f, _float3(0.f, 0.5f, 0.f) };
		m_vecShellData[static_cast<_uint>(SHELL::SHELL_SODACAN)] =
		{ "../Bin/DataFiles/PartObjectData/Shell_SodaCan", L"Prototype_Component_Model_Shell_SodaCan", L"Prototype_Component_ShellSkill_Fizzle", 1.f, _float3(0.f, 0.5f, 0.f) };
		m_vecShellData[static_cast<_uint>(SHELL::SHELL_MIRRORBALL)] =
		{ "../Bin/DataFiles/PartObjectData/Shell_MirrorBall", L"Prototype_Component_Model_Shell_MirrorBall", L"Prototype_Component_ShellSkill_RollOut", 1.f, _float3(0.f, 0.5f, 0.f)};
		m_vecShellData[static_cast<_uint>(SHELL::SHELL_DUCK)] =
		{ "../Bin/DataFiles/PartObjectData/Shell_Duck", L"Prototype_Component_Model_Shell_Duck", L"Prototype_Component_ShellSkill_Fizzle", 1.f, _float3(0.f, 0.5f, 0.f) };
		m_vecShellData[static_cast<_uint>(SHELL::SHELL_TENNISBALL)] =
		{ "../Bin/DataFiles/PartObjectData/Shell_TennisBall", L"Prototype_Component_Model_Shell_TennisBall", L"Prototype_Component_ShellSkill_RollOut", 1.f, _float3(0.f, 0.5f, 0.f) };
	}

public :
	static CShell_DataBase& Get()
	{
		static CShell_DataBase Instance;
		return Instance;
	}

	const FShellStat& Get_Stat(SHELL eShell) const
	{
		return m_vecShellStat[static_cast<_uint>(eShell)];
	}
	const FShellData& Get_Data(SHELL eShell) const
	{
		return m_vecShellData[static_cast<_uint>(eShell)];
	}

private :
	vector<FShellStat> m_vecShellStat;
	vector<FShellData> m_vecShellData;
};

END