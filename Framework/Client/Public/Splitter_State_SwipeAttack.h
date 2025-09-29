#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CSplitter_State_SwipeAttack final : public CState
{
	using super = CState;

private:
	CSplitter_State_SwipeAttack();
	virtual ~CSplitter_State_SwipeAttack() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;	

private:	
	_float	m_fCurActionDelay{ 0.f };
	_float	m_fMaxActionDelay{ 1.f };

public:
	static CSplitter_State_SwipeAttack* Create(void* pObj);
	virtual void Free() override;
};
END	