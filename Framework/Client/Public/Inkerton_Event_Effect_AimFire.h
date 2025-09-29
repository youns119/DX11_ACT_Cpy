#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Effect_AimFire final : public CEvent
{
private:
	CInkerton_Event_Effect_AimFire(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsFire);
	virtual ~CInkerton_Event_Effect_AimFire() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pSocketMatrix{};
	_bool m_bIsFire{};
public:
	static CInkerton_Event_Effect_AimFire* Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsFire);
	virtual void Free() override;
};

END