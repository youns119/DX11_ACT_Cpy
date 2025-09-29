#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CRoland_State_SideWalk final : public CState
{
	using super = CState;

private:
	CRoland_State_SideWalk();
	virtual ~CRoland_State_SideWalk() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_int	m_iAnimIndex{ 0 };	

public:
	static CRoland_State_SideWalk* Create(void* pArg);
	virtual void Free() override;
};
END	