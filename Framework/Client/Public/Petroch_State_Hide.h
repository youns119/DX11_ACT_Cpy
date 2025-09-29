#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CBoss_Petroch;

class CPetroch_State_Hide final : public CState
{
	using super = CState;

private:
	CPetroch_State_Hide();
	virtual ~CPetroch_State_Hide() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Call_ParryEffect();

private:
	_int	m_iAnimIndex{ 0 };

private:
	CBoss_Petroch* m_pPetroch{ nullptr };

public:
	static CPetroch_State_Hide* Create(void* pArg);
	virtual void Free() override;
};
END	