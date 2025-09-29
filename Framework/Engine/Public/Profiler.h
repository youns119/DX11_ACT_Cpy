#pragma once

#include "Base.h"
#include "ScopeTimer.h"
#include "DoubleBuffer.h"


BEGIN(Engine)
class CProfiler final : public CBase
{
	using super = CBase;

public:
	struct RECORD
	{
		_wstring strTag;
		_double fMilliSecond { 0.f };
		_int iDepth { 0 };
	};

	struct STATISTICS
	{
		_double fTotalSum { 0.f };
		_double fAverage { 0.f };
		_double fMin { 999.f };
		_double fMax { 0.f };

		_uint iCallCount { 0 };
		_float fAverageCallCount { 0.f };
	};

private:
	CProfiler();
	CProfiler(const CProfiler& rhs) = delete;
	virtual ~CProfiler() = default;

public:
	CScopeTimer Profile_Scope(const _wstring& strTag);
	_bool Flush_Output(_uint iAverageInterval = 50);
	_bool Flush(_tchar* pBuffer, size_t iBufferLength, _uint iAverageInterval = 50);
	_bool Flush(_Out_ _wstring& strOutBuffer, _uint iAverageInterval = 50);
	_bool Record(const _wstring& strTag, _double fElapsedTime);
	void Count_Frame() { ++m_iFrameCountAcc; }

private:
	_int m_iStackCount{ 0 };
	LARGE_INTEGER m_CpuTick{};

	vector<RECORD> m_Records;
	map<_wstring, CDoubleBuffer<STATISTICS>> m_Statistics;

	_uint m_iFrameCountAcc{ 0 };

private:
	vector<_uint> Convert_PreOrder_To_PostOrder();
	_wstring Write_ScopeTimerLog(const vector<_uint>& SortedIndices, _uint iAverageInterval);

public:
	static CProfiler* Create();
	virtual void Free() override;
};
END
