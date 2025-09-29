#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Effect_MixupHeavy final : public CEvent
{
private:
	CInkerton_Event_Effect_MixupHeavy(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix);
	virtual ~CInkerton_Event_Effect_MixupHeavy() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pSocketMatrix{};

public:
	static CInkerton_Event_Effect_MixupHeavy* Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix);
	virtual void Free() override;
};

END