#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_Skill_RollOut final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Skill_RollOut();
	virtual ~CEvent_Skill_RollOut() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject) override;

public:
	virtual HRESULT Execute() override;

private :
	CMovement* m_pMovement{};

private :
	CPlayer* m_pPlayer{};

public:
	static CEvent_Skill_RollOut* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END