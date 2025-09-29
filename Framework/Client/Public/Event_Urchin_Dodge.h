#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_Urchin_Dodge final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Urchin_Dodge();
	virtual ~CEvent_Urchin_Dodge() = default;

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
	static CEvent_Urchin_Dodge* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END