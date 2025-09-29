#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CBowman_State_Backflip final : public CState
{
	using super = CState;

private:
	CBowman_State_Backflip();
	virtual ~CBowman_State_Backflip() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:	
	static CBowman_State_Backflip* Create(void* pArg);
	virtual void Free() override;
};
END	