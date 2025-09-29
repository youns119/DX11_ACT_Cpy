#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

struct FPlayerStat;

class CEvent_Urchin_Launch final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Urchin_Launch();
	virtual ~CEvent_Urchin_Launch() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CModel* m_pModel{};
	CMovement* m_pMovement{};

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_Urchin_Launch* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END