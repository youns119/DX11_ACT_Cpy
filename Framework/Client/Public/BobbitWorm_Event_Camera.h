#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CCineCamera;
END

BEGIN(Client)

class CBobbitWorm_Event_Camera final : public CEvent
{
private:
	CBobbitWorm_Event_Camera();
	virtual ~CBobbitWorm_Event_Camera() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);
	HRESULT Execute() override;
	
private:
	CCineCamera* m_pCineCamera{};

public:
	static CBobbitWorm_Event_Camera* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	