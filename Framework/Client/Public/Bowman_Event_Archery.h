#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CBowman_Event_Archery final : public CEvent
{
private:
	CBowman_Event_Archery();
	virtual ~CBowman_Event_Archery() = default;

public:
	HRESULT Execute() override;

private:
	CGameInstance* m_pGameInstance{ nullptr };


public:
	static CBowman_Event_Archery* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END	