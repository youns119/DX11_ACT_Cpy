#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CInkerton_Event_MoveSpline final : public CEvent
{
private:
	CInkerton_Event_MoveSpline(_bool _bIsMove, _float _fDuraton = 0);
	virtual ~CInkerton_Event_MoveSpline() = default;

public:
	HRESULT Execute() override;

public:
	_float Compute_Distance(_vec2 vA, _vec2 vB);
	
private:
	_float			m_fDuraton{ 0.f };
	_float			m_fDisatance{ 0.f };
	vector<_vec2>	m_Routes{};

private:
	_bool			m_bIsMove;

public:
	static CInkerton_Event_MoveSpline* Create(CGameObject* _pGameObject, _bool _bIsMove, _float _fDuraton = 0);
	virtual void Free() override;
};
END	