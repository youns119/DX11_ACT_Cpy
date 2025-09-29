#pragma once
#include "Client_Defines.h"
#include "Event.h"
BEGIN(Client)

class CEvent_Effect_Finish : public CEvent
{
	using super = CEvent;

private:
	CEvent_Effect_Finish();
	virtual ~CEvent_Effect_Finish() = default;

public:
	HRESULT Init(CGameObject* pGameObject, _bool bIsBlood);

public:
	virtual HRESULT Execute() override;

private:
	_bool m_bIsBlood = {};

public:
	static CEvent_Effect_Finish* Create(CGameObject* pGameObject, _bool bIsBlood);
	virtual void Free() override;
};

END