#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_BackStepAttack_Sound final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_BackStepAttack_Sound();
	virtual ~CEvent_BackStepAttack_Sound() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_BackStepAttack_Sound* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END