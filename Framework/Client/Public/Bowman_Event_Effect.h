#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CBowman_Event_Effect final : public CEvent
{
public:
	enum BOWMAN_EFFECT { JUMP, DISTORTION, BASH, CLAW, BOWMAN_EFFECT_END };

private:
	CBowman_Event_Effect(CGameObject* _pGameObject, _uint iEffectType);
	virtual ~CBowman_Event_Effect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pParentWorldMatrix{ nullptr };
	const _float4x4* m_pRubberBand{ nullptr };
	const _float4x4* m_pClawArm{ nullptr };

	_uint				m_iEffectType{};

public:
	static CBowman_Event_Effect* Create(CGameObject* _pGameObject, _uint iEffectType);
	virtual void Free() override;
};
END