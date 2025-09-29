#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CEvent_Effect_ChargeAtt : public CEvent
{
	using super = CEvent;

private:
	CEvent_Effect_ChargeAtt();
	virtual ~CEvent_Effect_ChargeAtt() = default;

public:
	HRESULT Init(CGameObject* pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix);

public:
	virtual HRESULT Execute() override;

private:
	const _float4x4* m_pParentWorldMatrix = { nullptr };
	const _float4x4* m_pCombinedWorldMatrix = { nullptr };

public:
	static CEvent_Effect_ChargeAtt* Create(CGameObject* pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix);
	virtual void Free() override;
};

END