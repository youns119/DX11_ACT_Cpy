#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CSardine_State_Wait final : public CState
{
	using super = CState;

private:
	CSardine_State_Wait();
	virtual ~CSardine_State_Wait() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
private:	
	CMovement*		m_pMovement = { nullptr };	
	CTransform*		m_pPlayerTransform = { nullptr };

public:
	static CSardine_State_Wait* Create(void* pArg);
	virtual void Free() override;
};
END	