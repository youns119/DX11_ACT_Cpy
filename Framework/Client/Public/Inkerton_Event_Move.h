#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Move final : public CEvent
{
private:
	CInkerton_Event_Move(_float _fSpeed, _bool _bIsChase);
	virtual ~CInkerton_Event_Move() = default;

public:
	HRESULT Execute() override;
	
private:
	_float m_fSpeed = { 1 };
	_bool m_bIsChase = { false };

public:
	static CInkerton_Event_Move* Create(CGameObject* _pGameObject, _bool _bIsChase = false, _float _fSpeed = 0);
	virtual void Free() override;
};
END	