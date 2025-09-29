#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CPetroch_Event_Effect final : public CEvent
{
public:
	enum PETROCH_EFFECT {CHARGE, JUMP, GROUNDSLAM, POUNCE, PETROCH_EFFECT_END};

private:
	CPetroch_Event_Effect(CGameObject* _pGameObject, _uint iEffectType);
	virtual ~CPetroch_Event_Effect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4*	m_pParentWorldMatrix{ nullptr };
	const _float4x4*	m_pMidLegFootL{ nullptr };
	const _float4x4*	m_pMidLegFootR{ nullptr };
	const _float4x4*	m_pBackLegFootL{ nullptr };
	const _float4x4*	m_pBackLegFootR{ nullptr };
	const _float4x4*	m_pArmL{ nullptr };
	const _float4x4*	m_pArmR{ nullptr };

	_uint				m_iEffectType{};

public:
	static CPetroch_Event_Effect* Create(CGameObject* _pGameObject, _uint iEffectType);
	virtual void Free() override;
};
END