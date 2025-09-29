#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CNormie_State_Walk final : public CState
{
	using super = CState;

private:
	CNormie_State_Walk();
	virtual ~CNormie_State_Walk() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
private:
	CMovement* m_pMovement = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	CTransform* m_pTransform = { nullptr };

public:
	static CNormie_State_Walk* Create(void* pArg);
	virtual void Free() override;
};
END	