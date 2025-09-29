#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* ���۷��� ī��Ʈ�� ������Ű��. */
	_uint AddRef();

	/* ���۷��� ī��Ʈ�� ���ҽ�Ų��. or �����Ѵ�. */
	_uint Release();
private:
	_uint			m_iRefCnt = {};

public:
	virtual void Free();
};

END