#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CModel;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_DashAttackEnd final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_DashAttackEnd();
	virtual ~CEvent_DashAttackEnd() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CMovement* m_pMovement{};

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_DashAttackEnd* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END