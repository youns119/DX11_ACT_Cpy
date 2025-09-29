#pragma once

#include "Base.h"

BEGIN(Engine)

template<typename T>
class ENGINE_DLL CShared final : public CBase
{
private:
	CShared() : m_Value { T{} } {}
	CShared(const T& Prototype) : m_Value { Prototype } {}
	CShared(T&& Prototype) : m_Value { std::move(Prototype) } {} // move 사용에 대해서는 좀 더 공부해보기 // batch 랑 effect가 생성자로 && 인자만 받는다. .ㅂ.
	virtual ~CShared() = default;

public:
	T* Ptr() { return &m_Value; }
	T& Value() { return m_Value; }


	T* operator&()
	{
		return &m_Value;
	}
	T operator*()
	{
		return m_Value;
	}
	T* operator->()
	{
		return &m_Value;
	}

	static CShared<T>* Create(T&& Prototype)
	{
		CShared<T>* pInstance = new CShared<T>(std::move(Prototype));

		return pInstance;
	}

	static CShared<T>* Create(const T& Prototype)
	{
		CShared<T>* pInstance = new CShared<T>(Prototype);

		return pInstance;
	}

	static CShared<T>* Create()
	{
		return new CShared<T>();
	}



private:
	T m_Value;
};

END
