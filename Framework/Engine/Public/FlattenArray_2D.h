#pragma once

#include "Engine_Defines.h"

template<typename T>
class CFlattenArray_2D
{
public:
	CFlattenArray_2D(size_t iWidth, size_t iHeight)
		: m_iWidth(iWidth)
		, m_iHeight(iHeight)
		, m_Array(iWidth* iHeight)
	{
	}

	T& at(size_t x, size_t y)
	{
		Check_OutOfRange(x, y);
		return m_Array[y * m_iWidth + x];
	}

	const T& at(size_t x, size_t y) const
	{
		Check_OutOfRange(x, y);
		return m_Array[y * m_iWidth + x];
	}

	T& operator()(size_t x, size_t y) { return at(x, y); }
	const T& operator()(size_t x, size_t y) const { return at(x, y); }

	T* Data() { return m_Array.data(); }
	const T* Data() const { return m_Array.data(); }

	size_t Width() const { return m_iWidth; }
	size_t Height() const { return m_iHeight; }
	size_t Size() const { return m_Array.size(); }

private:
	void Check_OutOfRange(size_t iX, size_t iY) const 
	{
#ifdef _DEBUG
		assert(iX < m_iWidth && iY < m_iHeight);
#endif
	}

	size_t m_iWidth;
	size_t m_iHeight;

	std::vector<T> m_Array;
};
