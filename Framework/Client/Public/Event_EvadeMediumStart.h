#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

struct FPlayerStat;

class CEvent_EvadeMediumStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_EvadeMediumStart();
	virtual ~CEvent_EvadeMediumStart() = default;

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
	static CEvent_EvadeMediumStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END