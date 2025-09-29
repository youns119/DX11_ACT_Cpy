#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CPlayer;

class CEvent_Effect_BeforeDash : public CEvent
{
	using super = CEvent;

private:
	CEvent_Effect_BeforeDash();
	virtual ~CEvent_Effect_BeforeDash() = default;

public:
	HRESULT Init(CGameObject* pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix);

public:
	virtual HRESULT Execute() override;

private:
	const _float4x4* m_pParentWorldMatrix = { nullptr };
	const _float4x4* m_pCombinedWorldMatrix = { nullptr };

private :
	CPlayer* m_pPlayer{};

public:
	static CEvent_Effect_BeforeDash* Create(CGameObject* pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix);
	virtual void Free() override;
};

END