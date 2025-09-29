#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_WallopEffect final : public CEvent
{
private:
	CRoland_Event_WallopEffect(CGameObject* _pGameObject, _bool _bIsHit, const _float4x4* _pCombinedWorldMatrix);
	virtual ~CRoland_Event_WallopEffect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pCombinedWorldMatrix{nullptr};
	_bool	m_bIsHitEffect{};

public:
	static CRoland_Event_WallopEffect* Create(CGameObject* _pGameObject, _bool _bIsHit, const _float4x4* _pCombinedWorldMatrix);
	virtual void Free() override;
};

END