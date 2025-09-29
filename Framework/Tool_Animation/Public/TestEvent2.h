#pragma once

#include "ToolAnimation_Defines.h"

#include "Event.h"

BEGIN(ToolAnimation)

class CAnimObject;

class CTestEvent2 final
	: public CEvent
{
private:
	CTestEvent2();
	virtual ~CTestEvent2() = default;

public:
	virtual HRESULT Execute() override;

public:
	static CTestEvent2* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END