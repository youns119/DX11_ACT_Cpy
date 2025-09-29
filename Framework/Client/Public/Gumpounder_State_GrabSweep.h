#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CGumpounder_State_GrabSweep final : public CState
{
	using super = CState;

private:
	CGumpounder_State_GrabSweep();
	virtual ~CGumpounder_State_GrabSweep() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:
	static CGumpounder_State_GrabSweep* Create(void* pArg);
	virtual void Free() override;
};
END	