#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CEvent_MantisPunch_Retreat final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_MantisPunch_Retreat();
	virtual ~CEvent_MantisPunch_Retreat() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private :
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

public:
	static CEvent_MantisPunch_Retreat* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END