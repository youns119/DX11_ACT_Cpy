#pragma once

template<typename T>
class CDoubleBuffer
{
public:
	CDoubleBuffer()
		: m_pRead { &m_Src }
		, m_pWrite { &m_Dst }
	{}
	CDoubleBuffer(CDoubleBuffer&& Other)
		: m_Dst { Other.m_Dst }
		, m_Src { Other.m_Src }
		, m_pRead { &m_Src }
		, m_pWrite { &m_Dst }
	{}
	CDoubleBuffer(const CDoubleBuffer& Other)
		: m_Dst { Other.m_Dst }
		, m_Src { Other.m_Src }
		, m_pRead { &m_Src }
		, m_pWrite { &m_Dst }
	{}

	~CDoubleBuffer() = default;

public:
	const T& Read() const { return *m_pRead; }
	T& Write() { return *m_pWrite; }
	void Swap() 
	{ 
		std::swap(m_pRead, m_pWrite); 
	}

private:
	T m_Src {};
	T m_Dst {};

	T* m_pRead;
	T* m_pWrite;
};

