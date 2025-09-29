#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)
class CProfiler;

class ENGINE_DLL CScopeTimer
{
public:
	CScopeTimer() = delete;
	CScopeTimer(const CScopeTimer& other) = delete;
	CScopeTimer(CScopeTimer&& other) noexcept;
	CScopeTimer(const _wstring& strTag, LARGE_INTEGER CpuTick, CProfiler* pProfiler);
	~CScopeTimer();

private:
	const LARGE_INTEGER			m_CpuTick;
	const _wstring				m_strTag;
	CProfiler*					m_pProfiler { nullptr };

	LARGE_INTEGER				m_BeginTime { 0 };

	_bool						m_isMoved { false };
};
END
