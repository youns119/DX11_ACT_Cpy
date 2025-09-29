#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_BackStepStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_BackStepStart();
	virtual ~CEvent_BackStepStart() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_BackStepStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END