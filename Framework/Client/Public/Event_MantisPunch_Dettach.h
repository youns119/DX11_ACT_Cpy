#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CSkill_Topoda;

class CEvent_MantisPunch_Dettach final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_MantisPunch_Dettach();
	virtual ~CEvent_MantisPunch_Dettach() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CSkill_Topoda* m_pTopoda{};

public:
	static CEvent_MantisPunch_Dettach* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END