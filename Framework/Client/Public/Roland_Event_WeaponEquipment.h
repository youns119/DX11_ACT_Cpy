#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_WeaponEquipment final : public CEvent
{
private:
	CRoland_Event_WeaponEquipment(_bool _bIsShowing);
	virtual ~CRoland_Event_WeaponEquipment() = default;

public:
	HRESULT Execute() override;
	
private:
	_bool m_bIsShowing{ false };

public:
	static CRoland_Event_WeaponEquipment* Create(CGameObject* _pGameObject, _bool _bIsShowing = false);
	virtual void Free() override;
};
END	