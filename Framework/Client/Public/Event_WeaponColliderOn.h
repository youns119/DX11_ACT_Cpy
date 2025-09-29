#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_WeaponColliderOn final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_WeaponColliderOn();
	virtual ~CEvent_WeaponColliderOn() = default;

public :
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private :
	CPlayer* m_pPlayer{};

public:
	static CEvent_WeaponColliderOn* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END