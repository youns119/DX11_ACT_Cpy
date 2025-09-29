#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CCuttlefish_Event_Effect final : public CEvent
{
public:
	enum CUTTLE_EFFECT { INK, LEFT_CHARGE, RIGHT_CHARGE, PUNCH_READY, PUNCH, PUNCH_IMPACT, GRAB, CUTTLE_EFFECT_END };

private:
	CCuttlefish_Event_Effect(CGameObject* _pGameObject, _uint iEffectType);
	virtual ~CCuttlefish_Event_Effect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pParentWorldMatrix{ nullptr };
	const _float4x4* m_pLeftArm{ nullptr };
	const _float4x4* m_pRightArm{ nullptr };
	const _float4x4* m_pPunchMatrix{ nullptr };

	_uint				m_iEffectType{};

public:
	static CCuttlefish_Event_Effect* Create(CGameObject* _pGameObject, _uint iEffectType);
	virtual void Free() override;
};
END