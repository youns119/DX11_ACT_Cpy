#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CMovement;
class CTransform;
END

BEGIN(Client)

class CRoland_State_Walk final : public CState
{
	using super = CState;

private:
	CRoland_State_Walk();
	virtual ~CRoland_State_Walk() = default;

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
	static CRoland_State_Walk* Create(void* pArg);
	virtual void Free() override;
};
END	