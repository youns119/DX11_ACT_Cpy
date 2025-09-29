#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_ParryAttackFinish final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_ParryAttackFinish();
	virtual ~CEvent_ParryAttackFinish() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_ParryAttackFinish* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END