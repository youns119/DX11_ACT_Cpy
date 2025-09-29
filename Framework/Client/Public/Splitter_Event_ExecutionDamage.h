#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CSplitter_LeftClaw;

class CSplitter_Event_ExecutionDamage final 
	: public CEvent
{
	using super = CEvent;

private:
	CSplitter_Event_ExecutionDamage(_float _Damage);
	virtual ~CSplitter_Event_ExecutionDamage() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private :
	CSplitter_LeftClaw* m_pLeftClaw{};

private:
	_float m_fDamage{ 10.f };

public:
	static CSplitter_Event_ExecutionDamage* Create(CGameObject* _pGameObject, _float _Damage);
	virtual void Free() override;
};
END	