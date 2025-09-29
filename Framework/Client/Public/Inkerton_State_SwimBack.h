#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CInkerton_State_SwimBack final : public CState
{
	using super = CState;

private:
	CInkerton_State_SwimBack();
	virtual ~CInkerton_State_SwimBack() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:
	static CInkerton_State_SwimBack* Create(void* pArg);
	virtual void Free() override;
};
END	