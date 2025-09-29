#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CEvent_NPC_Effect_Inkerton final : public CEvent
{
public:
	enum NPCINK_EFFECT_TYPE {FLY, CHARGE, IMPACT, TYPE_END};
private:
	CEvent_NPC_Effect_Inkerton(CGameObject* _pGameObject, _uint _iEventType);
	virtual ~CEvent_NPC_Effect_Inkerton() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4*	m_pSocketMatrix{nullptr};
	_uint				m_iEventType{};

public:
	static CEvent_NPC_Effect_Inkerton* Create(CGameObject* _pGameObject, _uint _iEventType);
	virtual void Free() override;
};

END