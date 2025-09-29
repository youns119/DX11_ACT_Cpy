#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Effect_ChargeAttack final : public CEvent
{
private:
	CInkerton_Event_Effect_ChargeAttack(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix);
	virtual ~CInkerton_Event_Effect_ChargeAttack() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pSocketMatrix{};

public:
	static CInkerton_Event_Effect_ChargeAttack* Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix);
	virtual void Free() override;
};

END