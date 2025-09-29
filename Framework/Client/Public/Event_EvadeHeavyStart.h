#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

struct FPlayerStat;

class CEvent_EvadeHeavyStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_EvadeHeavyStart();
	virtual ~CEvent_EvadeHeavyStart() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_EvadeHeavyStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END