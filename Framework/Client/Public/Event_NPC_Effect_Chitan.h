#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CEvent_NPC_Effect_Chitan final : public CEvent
{
public:
	enum NPCCHI_EFFECT_TYPE { SANDBURST, IMPACT_DOUBLE, TYPE_END };
private:
	CEvent_NPC_Effect_Chitan(CGameObject* _pGameObject, _uint _iEventType);
	virtual ~CEvent_NPC_Effect_Chitan() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4*	m_pLeftSocketMatrix{};
	const _float4x4*	m_pRightSocketMatrix{};
	_uint				m_iEventType{};

public:
	static CEvent_NPC_Effect_Chitan* Create(CGameObject* _pGameObject, _uint _iEventType);
	virtual void Free() override;
};

END