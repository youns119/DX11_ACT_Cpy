#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_MoveSide final : public CEvent
{
private:
	CInkerton_Event_MoveSide(_float _fSpeed, _bool _bIsSideMove);
	virtual ~CInkerton_Event_MoveSide() = default;

public:
	HRESULT Execute() override;
	
private:
	_float m_fSpeed = { 1 };
	_bool m_bIsSideMove = { false };

public:
	static CInkerton_Event_MoveSide* Create(CGameObject* _pGameObject, _bool _bIsSideMove = false, _float _fSpeed = 0);
	virtual void Free() override;
};
END	