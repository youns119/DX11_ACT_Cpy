#pragma once

#include "Engine_Defines.h"

template<typename T>
class CFlattenArray_3D
{
public:
	CFlattenArray_3D(size_t iWidth, size_t iHeight, size_t iDepth)
		: m_iWidth(iWidth)
		, m_iHeight(iHeight)
		, m_iDepth(iDepth)
		, m_Array(iWidth * iHeight * iDepth)
	{
	}

	T& at(size_t x, size_t y, size_t z) 
	{
		Check_OutOfRange(x, y, z);
		return m_Array[z * m_iWidth * m_iHeight + y * m_iWidth + x];
	}

	const T& at(size_t x, size_t y, size_t z) const 
	{
		Check_OutOfRange(x, y, z);
		return m_Array[z * m_iWidth * m_iHeight + y * m_iWidth + x];
	}

	T& operator()(size_t x, size_t y, size_t z) { return at(x, y, z); }
	const T& operator()(size_t x, size_t y, size_t z) const { return at(x, y, z); }

	T* Data() { return m_Array.data(); }
	const T* Data() const { return m_Array.data(); }

	size_t Width() const { return m_iWidth; }
	size_t Height() const { return m_iHeight; }
	size_t Depth() const { return m_iDepth; }
	size_t Size() const { return m_Array.size(); }

private:
	void Check_OutOfRange(size_t iX, size_t iY, size_t iZ) const 
	{
#ifdef _DEBUG
		assert(iX < m_iWidth && iY < m_iHeight && iX < m_iDepth);
#endif
	}

	size_t m_iWidth;
	size_t m_iHeight;
	size_t m_iDepth;

	std::vector<T> m_Array;
};
