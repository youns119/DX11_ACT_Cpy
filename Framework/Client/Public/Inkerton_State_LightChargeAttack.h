#pragma once
#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CInkerton_State_LightChargeAttack final : public CState
{
	using super = CState;

private:
	CInkerton_State_LightChargeAttack();
	virtual ~CInkerton_State_LightChargeAttack() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_bool After_AttackDelay(_float fTimeDelta);

private:
	_bool	m_bIsAnimChanged{ false };	
	_float	m_fCurActionDelay{ 0.f };
	_float	m_fMaxActionDelay{ 1.f };
	
public:
	static CInkerton_State_LightChargeAttack* Create(void* pArg);
	virtual void Free() override;
};
END	