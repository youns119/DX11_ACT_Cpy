#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_GrappleAttackFinish final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_GrappleAttackFinish();
	virtual ~CEvent_GrappleAttackFinish() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_GrappleAttackFinish* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END