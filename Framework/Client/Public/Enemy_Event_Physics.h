#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CPhysics_Controller;
END

BEGIN(Client)

class CEnemy_Event_Physics final : public CEvent
{
private:
	CEnemy_Event_Physics(CPhysics_Controller* _pCollider, _bool _bIsActive);
	virtual ~CEnemy_Event_Physics() = default;

public:
	HRESULT Execute() override;
	
private:
	CPhysics_Controller* m_pPhysicsController{ nullptr };

private:
	_bool m_bIsActive;

public:
	static CEnemy_Event_Physics* Create(CGameObject* _pGameObject, CPhysics_Controller* _pCollider, _bool _bIsActive);
	virtual void Free() override;
};
END	