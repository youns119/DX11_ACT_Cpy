#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CSplitter_State_Wait final : public CState
{
	using super = CState;

private:
	CSplitter_State_Wait();
	virtual ~CSplitter_State_Wait() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:
	static CSplitter_State_Wait* Create(void* pObj);
	virtual void Free() override;
};
END	