#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CInkerton_State_InkSpurt final : public CState
{
	using super = CState;

private:
	CInkerton_State_InkSpurt();
	virtual ~CInkerton_State_InkSpurt() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_bool After_AttackDelay(_float fTimeDelta);

private:
	_int	m_iAnimIndex{ 0 };
	_bool	m_bIsAnimChanged{ false };
	_float	m_fChaseSpeed{ 0.5f };
	_float	m_fCurActionDelay{ 0.f };
	_float	m_fMaxActionDelay{ 1.f };
	
public:
	static CInkerton_State_InkSpurt* Create(void* pArg);
	virtual void Free() override;
};
END	