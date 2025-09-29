#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_BackStepAttackStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_BackStepAttackStart();
	virtual ~CEvent_BackStepAttackStart() = default;

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
	static CEvent_BackStepAttackStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END