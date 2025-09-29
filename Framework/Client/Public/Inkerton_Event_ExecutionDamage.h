#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Client)

class CInkerton_Nutcracker;
class CInkerton_StretchedNutcracker;

class CInkerton_Event_ExecutionDamage final 
	: public CEvent
{
	using super = CEvent;

private:
	CInkerton_Event_ExecutionDamage(_float _fDamage, _float _fShellDamage);
	virtual ~CInkerton_Event_ExecutionDamage() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	HRESULT Execute() override;
	
private :
	CInkerton_Nutcracker* m_pInkerton_Nutcracker{};
	CInkerton_StretchedNutcracker* m_pInkerton_StretchedNutcracker{};

private:
	_float m_fDamage{ 10.f };
	_float m_fShellDamage{ 10.f };

public:
	static CInkerton_Event_ExecutionDamage* Create(CGameObject* _pGameObject, _float _fDamage, _float _fShellDamage);
	virtual void Free() override;
};
END	