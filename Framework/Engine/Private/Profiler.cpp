#include "Profiler.h"

CProfiler::CProfiler()
{
	QueryPerformanceFrequency(&m_CpuTick);
}

CScopeTimer CProfiler::Profile_Scope(const _wstring& strTag)
{
	++m_iStackCount;

	auto iter = m_Statistics.find(strTag);
	if (m_Statistics.end() == iter)
		m_Statistics.insert({ strTag, CDoubleBuffer<STATISTICS>() });
		//m_Statistics.emplace(strTag, CDoubleBuffer<STATISTICS>{});

	return move(CScopeTimer(strTag, m_CpuTick, this));
}

_bool CProfiler::Flush_Output(_uint iAverageInterval)
{
	vector<_uint> SortedIndices = Convert_PreOrder_To_PostOrder();

	_wstring strTotalLog = Write_ScopeTimerLog(SortedIndices, iAverageInterval);

	OutputDebugString(strTotalLog.c_str());

	m_Records.clear();
	return true;
}

_bool CProfiler::Flush(_tchar* pBuffer, size_t iBufferLength, _uint iAverageInterval)
{
	vector<_uint> SortedIndices = Convert_PreOrder_To_PostOrder();

	_wstring strTotalLog = Write_ScopeTimerLog(SortedIndices, iAverageInterval);


	if (strTotalLog.length() > iBufferLength)
	{
		lstrcpy(pBuffer, L"Buffer Overrun");
		return false;
	}

	lstrcpy(pBuffer, strTotalLog.c_str());

	m_Records.clear();
	return true;
}

_bool CProfiler::Flush(_Out_ _wstring& strOutBuffer, _uint iAverageInterval)
{
	vector<_uint> SortedIndices = Convert_PreOrder_To_PostOrder();

	_wstring strTotalLog = Write_ScopeTimerLog(SortedIndices, iAverageInterval);

	strOutBuffer.swap(strTotalLog);
	m_Records.clear();
	return true;
}

_bool CProfiler::Record(const _wstring& strTag, _double fElapsedTime)
{
	--m_iStackCount;
	m_Records.push_back(RECORD {strTag, fElapsedTime, m_iStackCount});

	auto iter = m_Statistics.find(strTag);

	STATISTICS& Statistics = iter->second.Write();
	Statistics.fTotalSum += fElapsedTime;
	Statistics.fMin = min(Statistics.fMin, fElapsedTime);
	Statistics.fMax = max(Statistics.fMax, fElapsedTime);
	++Statistics.iCallCount;

	return true;
}

vector<_uint> CProfiler::Convert_PreOrder_To_PostOrder()
{
	vector<_uint> SortedIndices;
	vector<_uint> ComparedIndices;

	_int iIndex = static_cast<_int>(m_Records.size()) - 1;
	while (iIndex >= 0)
	{
		while (!ComparedIndices.empty() &&
			m_Records[iIndex].iDepth <= m_Records[ComparedIndices.back()].iDepth)
		{
			SortedIndices.push_back(ComparedIndices.back());
			ComparedIndices.pop_back();
		}

		ComparedIndices.push_back(iIndex);
		--iIndex;
	}
	reverse(SortedIndices.begin(), SortedIndices.end());
	ComparedIndices.insert(ComparedIndices.end(), SortedIndices.begin(), SortedIndices.end());

	return ComparedIndices;
}

_wstring CProfiler::Write_ScopeTimerLog(const vector<_uint>& SortedIndices, _uint iAverageInterval)
{
	_wstring strTotalLog;
	for (_uint i : SortedIndices)
	{
		RECORD& Record = m_Records[i];
		CDoubleBuffer<STATISTICS>& Buffer = m_Statistics.find(Record.strTag)->second;
		const STATISTICS& Read = Buffer.Read();


		_tchar szRecord[MAX_PATH];
		_wstring strIndent = _wstring(Record.iDepth * 2, L' ') + Record.strTag;
		swprintf_s(szRecord, MAX_PATH, L"[ Scope Timer ] %-48s [%.6lf ms]  ", strIndent.c_str(), Record.fMilliSecond);
		strTotalLog += _wstring(szRecord);

		_tchar szStatics[MAX_PATH] {};
		swprintf_s(szStatics, MAX_PATH, L"min: [%.6lf ms], max: [%.6lf ms], average: [%.6lf ms], average call: [%.2f times]\n", Read.fMin, Read.fMax, Read.fAverage, Read.fAverageCallCount);
		strTotalLog += _wstring(szStatics);

		if (iAverageInterval <= m_iFrameCountAcc)
		{
			STATISTICS& Write = Buffer.Write();
			Write.fAverage = Write.fTotalSum / m_iFrameCountAcc;
			Write.fAverageCallCount = Write.iCallCount / (_float)m_iFrameCountAcc;

			Buffer.Swap();
			Buffer.Write() = move(STATISTICS {});
		}
	}

	if (iAverageInterval <= m_iFrameCountAcc)
		m_iFrameCountAcc = 0;

	return strTotalLog;
}

CProfiler* CProfiler::Create()
{
	return new CProfiler();
}

void CProfiler::Free()
{
	super::Free();

	m_Records.clear();
}
