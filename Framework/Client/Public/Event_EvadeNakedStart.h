#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

struct FPlayerStat;

class CEvent_EvadeNakedStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_EvadeNakedStart();
	virtual ~CEvent_EvadeNakedStart() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

private :
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_EvadeNakedStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END