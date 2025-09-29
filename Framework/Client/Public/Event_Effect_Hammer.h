#pragma once
#include "Client_Defines.h"
#include "Event.h"
BEGIN(Client)

class CPlayer;

class CEvent_Effect_Hammer : public CEvent
{
	using super = CEvent;

public:
	enum HAMMER_EFFECT_TYPE { SWAP, CHARGE, JUMP, THRUST, THRUST_WALLOP, CHARGE_WALLOP, ONCE, TWICE, DASH, AIR, TYPE_END };
private:
	CEvent_Effect_Hammer();
	virtual ~CEvent_Effect_Hammer() = default;

public:
	HRESULT Init(CGameObject* pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType);

public:
	virtual HRESULT Execute() override;

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_uint m_iEffectType = {};

private :
	CPlayer* m_pPlayer{};

public:
	static CEvent_Effect_Hammer* Create(CGameObject* pGameObject, const _float4x4* pSocketMatrix, _uint iEffectType);
	virtual void Free() override;
};

END