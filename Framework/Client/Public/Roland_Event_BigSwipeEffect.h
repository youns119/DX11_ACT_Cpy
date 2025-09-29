#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_BigSwipeEffect final : public CEvent
{
public:
	enum SWIPE_SEQ {SWIPE_START, SWIPE_END, SWIPE_HIT, SEQ_END};
private:
	CRoland_Event_BigSwipeEffect(CGameObject* _pGameObject, _uint _iCombo);
	virtual ~CRoland_Event_BigSwipeEffect() = default;

public:
	HRESULT Execute() override;

private:
	_uint m_iComboCount{};

public:
	static CRoland_Event_BigSwipeEffect* Create(CGameObject* _pGameObject, _uint _iCombo);
	virtual void Free() override;
};

END