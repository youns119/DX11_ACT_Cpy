#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CEnemy_Event_Collider final : public CEvent
{
private:
	CEnemy_Event_Collider(CCollider* _pCollider, _bool _bIsActive);
	virtual ~CEnemy_Event_Collider() = default;

public:
	HRESULT Execute() override;
	
private:
	CCollider* m_pCollider{ nullptr };

private:
	_bool m_bIsActive;

public:
	static CEnemy_Event_Collider* Create(CGameObject* _pGameObject, CCollider* _pCollider, _bool _bIsActive);
	virtual void Free() override;
};
END	