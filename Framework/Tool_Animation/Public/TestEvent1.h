#pragma once

#include "ToolAnimation_Defines.h"

#include "Event.h"

BEGIN(ToolAnimation)

class CTestEvent1 final
	: public CEvent
{
private :
	CTestEvent1();
	virtual ~CTestEvent1() = default;

public :
	virtual HRESULT Execute() override;

public :
	static CTestEvent1* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END