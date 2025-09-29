#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Effect_Slash final : public CEvent
{
private:
	CInkerton_Event_Effect_Slash(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _uint _iCombo);
	virtual ~CInkerton_Event_Effect_Slash() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4*	m_pSocketMatrix{};
	_uint				m_iCombo{};

public:
	static CInkerton_Event_Effect_Slash* Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _uint _iCombo);
	virtual void Free() override;
};

END