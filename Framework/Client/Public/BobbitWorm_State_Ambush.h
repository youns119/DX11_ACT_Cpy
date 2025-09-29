#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CEnemy_BobbitWorm;

class CBobbitWorm_State_Ambush final : public CState
{
	using super = CState;

private:
	CBobbitWorm_State_Ambush();
	virtual ~CBobbitWorm_State_Ambush() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Call_Effects();

private:
	CEnemy_BobbitWorm*	m_pBobbitWorm{ nullptr };	

private:
	_int				m_iAnimIndex{ 0 };

public:
	static CBobbitWorm_State_Ambush* Create(void* pObj);
	virtual void Free() override;
};
END