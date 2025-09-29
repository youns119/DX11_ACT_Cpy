#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CEnemy_Event_AnimSpeed final : public CEvent
{
private:
	CEnemy_Event_AnimSpeed(_float _fSpeed);
	virtual ~CEnemy_Event_AnimSpeed() = default;

public:
	HRESULT Execute() override;
	
private:
	_float m_fSpeed = { 1 };

public:
	static CEnemy_Event_AnimSpeed* Create(CGameObject* _pGameObject, _float _fSpeed = 0);
	virtual void Free() override;
};
END	