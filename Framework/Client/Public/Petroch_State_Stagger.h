#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPetroch_State_Stagger final : public CState
{
	using super = CState;

private:
	CPetroch_State_Stagger();
	virtual ~CPetroch_State_Stagger() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_int	m_iAnimIndex{ 0 };

public:
	static CPetroch_State_Stagger* Create(void* pArg);
	virtual void Free() override;
};
END	