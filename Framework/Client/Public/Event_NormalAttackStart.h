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

class CEvent_NormalAttackStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_NormalAttackStart();
	virtual ~CEvent_NormalAttackStart() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

private :
	CPlayer* m_pPlayer{};
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_NormalAttackStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END