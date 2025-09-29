#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_ParryAttackImpact final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_ParryAttackImpact();
	virtual ~CEvent_ParryAttackImpact() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_ParryAttackImpact* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END