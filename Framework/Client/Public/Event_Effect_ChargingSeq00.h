#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_Effect_ChargingSeq00 : public CEvent
{
	using super = CEvent;

private:
	CEvent_Effect_ChargingSeq00();
	virtual ~CEvent_Effect_ChargingSeq00() = default;

public:
	HRESULT Init(CGameObject* pGameObject, const _float4x4* pCombinedWorldMatrix);

public:
	virtual HRESULT Execute() override;

private:
	const _float4x4* m_pCombinedWorldMatrix = { nullptr };

private :
	CPlayer* m_pPlayer{};

public:
	static CEvent_Effect_ChargingSeq00* Create(CGameObject* pGameObject, const _float4x4* pCombinedWorldMatrix);
	virtual void Free() override;
};

END