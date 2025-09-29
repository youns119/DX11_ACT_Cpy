#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;
struct FPlayerStat;

class CEvent_SwipeAttackStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_SwipeAttackStart();
	virtual ~CEvent_SwipeAttackStart() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

private :
	CPlayer* m_pPlayer{};
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_SwipeAttackStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END