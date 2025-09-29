#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CSardine_State_ChargeAttack final : public CState
{
	using super = CState;

private:
	CSardine_State_ChargeAttack();
	virtual ~CSardine_State_ChargeAttack() = default;

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
	_float	m_fSpeed = { 0.01f };
	_float	m_fCurActionDelay{ 0.f };
	_float	m_fMaxActionDelay{ 1.f };

	
public:
	static CSardine_State_ChargeAttack* Create(void* pArg);
	virtual void Free() override;
};
END	