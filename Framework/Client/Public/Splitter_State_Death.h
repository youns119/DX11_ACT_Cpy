#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CSplitter_State_Death final : public CState
{
	using super = CState;

private:
	CSplitter_State_Death();
	virtual ~CSplitter_State_Death() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
public:
	static CSplitter_State_Death* Create(void* pObj);
	virtual void Free() override;
};
END	