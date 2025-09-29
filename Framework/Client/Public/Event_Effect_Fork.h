#pragma once
#include "Client_Defines.h"
#include "Event.h"
BEGIN(Client)

class CEvent_Effect_Fork : public CEvent
{
	using super = CEvent;

private:
	CEvent_Effect_Fork();
	virtual ~CEvent_Effect_Fork() = default;

public:
	HRESULT Init(CGameObject* pGameObject, const _float4x4* pCombinedMatrix);

public:
	virtual HRESULT Execute() override;

private:
	const _float4x4* m_pCombinedMatrix{};

public:
	static CEvent_Effect_Fork* Create(CGameObject* pGameObject, const _float4x4* pCombinedMatrix);
	virtual void Free() override;
};

END