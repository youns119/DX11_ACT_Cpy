#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;
class CCamera_Player;

struct FPlayerStat;

class CEvent_Skill_Fizzle final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Skill_Fizzle();
	virtual ~CEvent_Skill_Fizzle() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject) override;

public:
	virtual HRESULT Execute() override;

public:
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

public :
	FPlayerStat* m_pPlayerStat{};

public:
	CPlayer* m_pPlayer{};

public:
	CCamera_Player* m_pPlayerCamera{};

public:
	static CEvent_Skill_Fizzle* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END