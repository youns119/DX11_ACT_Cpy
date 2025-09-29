#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CCuttlefish_State_UnaggroedWait final : public CState
{
	using super = CState;

private:
	CCuttlefish_State_UnaggroedWait();
	virtual ~CCuttlefish_State_UnaggroedWait() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:
	static CCuttlefish_State_UnaggroedWait* Create(void* pArg);
	virtual void Free() override;
};
END	