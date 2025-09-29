#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_ThrustEffect final : public CEvent
{
private:
	CRoland_Event_ThrustEffect(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix);
	virtual ~CRoland_Event_ThrustEffect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pCombinedWorldMatrix;

public:
	static CRoland_Event_ThrustEffect* Create(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix);
	virtual void Free() override;
};

END