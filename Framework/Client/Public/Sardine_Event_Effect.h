#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CSardine_Event_Effect final : public CEvent
{
private:
	CSardine_Event_Effect(CGameObject* _pGameObject);
	virtual ~CSardine_Event_Effect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pParentWorldMatrix{ nullptr };
	const _float4x4* m_pTailMatrix{ nullptr };

public:
	static CSardine_Event_Effect* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END