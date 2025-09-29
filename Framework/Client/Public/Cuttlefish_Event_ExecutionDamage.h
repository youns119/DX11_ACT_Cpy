#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CCuttlefish_GrabTentacle;

class CCuttlefish_Event_ExecutionDamage final 
	: public CEvent
{
	using super = CEvent;

private:
	CCuttlefish_Event_ExecutionDamage(_float _fDamage, _float _fShellDamage);
	virtual ~CCuttlefish_Event_ExecutionDamage() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private :
	CCuttlefish_GrabTentacle* m_pGrabTentacle{ nullptr };

private:
	_float m_fDamage{ 10.f };
	_float m_fShellDamage{ 10.f };

public:
	static CCuttlefish_Event_ExecutionDamage* Create(CGameObject* _pGameObject, _float _fDamage, _float _fShellDamage);
	virtual void Free() override;
};
END	