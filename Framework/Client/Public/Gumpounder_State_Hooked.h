#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CGumpounder_State_Hooked final : public CState
{
	using super = CState;

private:
	CGumpounder_State_Hooked();
	virtual ~CGumpounder_State_Hooked() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:
	static CGumpounder_State_Hooked* Create(void* pArg);
	virtual void Free() override;
};
END	