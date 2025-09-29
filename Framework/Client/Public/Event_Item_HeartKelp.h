#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

struct FPlayerStat;

class CEvent_Item_HeartKelp final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Item_HeartKelp();
	virtual ~CEvent_Item_HeartKelp() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	void Call_HeartKelpEffect();

private :
	CPlayer* m_pPlayer{};

public:
	static CEvent_Item_HeartKelp* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END