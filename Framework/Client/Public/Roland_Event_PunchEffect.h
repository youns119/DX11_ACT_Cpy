#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_PunchEffect final : public CEvent
{
private:
	CRoland_Event_PunchEffect(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix);
	virtual ~CRoland_Event_PunchEffect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pCombinedWorldMatrix;

public:
	static CRoland_Event_PunchEffect* Create(CGameObject* _pGameObject, const _float4x4* pCombinedWorldMatrix);
	virtual void Free() override;
};

END