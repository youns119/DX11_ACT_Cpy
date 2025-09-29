#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CSardine_State_Death final : public CState
{
	using super = CState;

private:
	CSardine_State_Death();
	virtual ~CSardine_State_Death() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

public:
	static CSardine_State_Death* Create(void* pArg);
	virtual void Free() override;
};
END	