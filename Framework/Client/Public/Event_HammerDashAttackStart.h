#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_HammerDashAttackStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_HammerDashAttackStart();
	virtual ~CEvent_HammerDashAttackStart() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_HammerDashAttackStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END