#include "ScopeTimer.h"
#include "Profiler.h"

CScopeTimer::CScopeTimer(CScopeTimer&& other) noexcept
	: m_strTag { other.m_strTag }
	, m_CpuTick { other.m_CpuTick }
	, m_pProfiler { other.m_pProfiler }
	, m_isMoved { false }
{
	other.m_isMoved = true;
	QueryPerformanceCounter(&m_BeginTime);
}

CScopeTimer::CScopeTimer(const _wstring& strTag, LARGE_INTEGER CpuTick, CProfiler* pProfiler)
	: m_strTag { strTag }
	, m_CpuTick { CpuTick }
	, m_pProfiler { pProfiler }
{
}

CScopeTimer::~CScopeTimer()
{
	if (true == m_isMoved)
		return;

	LARGE_INTEGER EndTime {};
	QueryPerformanceCounter(&EndTime);

	_double fMilliSecond = (EndTime.QuadPart - m_BeginTime.QuadPart) / static_cast<_double>(m_CpuTick.QuadPart) * 1000.;

	m_pProfiler->Record(m_strTag, fMilliSecond);
}
