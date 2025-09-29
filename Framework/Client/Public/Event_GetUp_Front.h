#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_GetUp_Front final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_GetUp_Front();
	virtual ~CEvent_GetUp_Front() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject) override;

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_GetUp_Front* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END