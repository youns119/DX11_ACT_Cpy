#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_SlashEffect final : public CEvent
{
private:
	CRoland_Event_SlashEffect(CGameObject* _pGameObject, _bool bIsLeft);
	virtual ~CRoland_Event_SlashEffect() = default;

public:
	HRESULT Execute() override;

private:
	_bool m_bIsLeft{};

public:
	static CRoland_Event_SlashEffect* Create(CGameObject* _pGameObject, _bool bIsLeft);
	virtual void Free() override;
};

END