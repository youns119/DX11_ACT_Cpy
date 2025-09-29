#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CNormie_State_Death final : public CState
{
	using super = CState;

private:
	CNormie_State_Death();
	virtual ~CNormie_State_Death() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:
	static CNormie_State_Death* Create(void* pArg);
	virtual void Free() override;
};
END	