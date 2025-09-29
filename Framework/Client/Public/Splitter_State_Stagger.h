#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CSplitter_State_Stagger final : public CState
{
	using super = CState;

private:
	CSplitter_State_Stagger();
	virtual ~CSplitter_State_Stagger() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:
	static CSplitter_State_Stagger* Create(void* pObjs);
	virtual void Free() override;
};
END	