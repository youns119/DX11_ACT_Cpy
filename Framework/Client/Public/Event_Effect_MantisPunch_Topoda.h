#pragma once
#include "Client_Defines.h"
#include "Event.h"
BEGIN(Client)

class CEvent_Effect_MantisPunch_Topoda : public CEvent
{
	using super = CEvent;

public:
	enum MANTISPUNCH_EFFECT_TYPE { CHARGE, TELE_START, TELE_END, SLASH, IMPACT, JUMP_OUT, TYPE_END };
private:
	CEvent_Effect_MantisPunch_Topoda();
	virtual ~CEvent_Effect_MantisPunch_Topoda() = default;

public:
	HRESULT Init(CGameObject* pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType);

public:
	virtual HRESULT Execute() override;

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_uint m_iEffectType = {};

public:
	static CEvent_Effect_MantisPunch_Topoda* Create(CGameObject* pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType);
	virtual void Free() override;
};

END