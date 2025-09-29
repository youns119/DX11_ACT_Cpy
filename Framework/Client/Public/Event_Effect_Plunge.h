#pragma once
#include "Client_Defines.h"
#include "Event.h"
BEGIN(Client)

class CEvent_Effect_Plunge : public CEvent
{
	using super = CEvent;

private:
	CEvent_Effect_Plunge();
	virtual ~CEvent_Effect_Plunge() = default;

public:
	HRESULT Init(CGameObject* pGameObject, const _float4x4* pSocketMatrix, _bool bIsImpact);

public:
	virtual HRESULT Execute() override;

private:
	const _float4x4* m_pSocketMatrix{};
	_bool m_bIsImpact = {};

public:
	static CEvent_Effect_Plunge* Create(CGameObject* pGameObject, const _float4x4* pSocketMatrix, _bool bIsImpact);
	virtual void Free() override;
};

END