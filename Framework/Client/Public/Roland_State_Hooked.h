#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CBoss_Roland;

class CRoland_State_Hooked final : public CState
{
	using super = CState;

private:
	CRoland_State_Hooked();
	virtual ~CRoland_State_Hooked() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	CBoss_Roland* m_pRoland{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

private:
	_int	m_iAnimIndex{ 0 };	

public:
	static CRoland_State_Hooked* Create(void* pArg);
	virtual void Free() override;
};
END	