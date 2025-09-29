#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Effect_Execution final : public CEvent
{
private:
	CInkerton_Event_Effect_Execution(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsSlam);
	virtual ~CInkerton_Event_Effect_Execution() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pSocketMatrix{};
	_bool				m_bIsSlam{};
public:
	static CInkerton_Event_Effect_Execution* Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsSlam);
	virtual void Free() override;
};

END