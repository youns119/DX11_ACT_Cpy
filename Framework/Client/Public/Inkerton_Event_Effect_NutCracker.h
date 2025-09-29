#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_Effect_NutCracker final : public CEvent
{
private:
	CInkerton_Event_Effect_NutCracker(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsReady);
	virtual ~CInkerton_Event_Effect_NutCracker() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4*	m_pSocketMatrix{};
	_bool				m_bIsReady{};
public:
	static CInkerton_Event_Effect_NutCracker* Create(CGameObject* _pGameObject, const _float4x4* _pSocketMatrix, _bool _bIsReady);
	virtual void Free() override;
};

END