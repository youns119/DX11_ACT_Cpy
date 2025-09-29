#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_MantisPunch_Teleport final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_MantisPunch_Teleport();
	virtual ~CEvent_MantisPunch_Teleport() = default;

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
	static CEvent_MantisPunch_Teleport* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END