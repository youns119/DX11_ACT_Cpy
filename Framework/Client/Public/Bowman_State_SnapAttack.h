#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CBowman_State_SnapAttack final : public CState
{
	using super = CState;

private:
	CBowman_State_SnapAttack();
	virtual ~CBowman_State_SnapAttack() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_bool After_AttackDelay(_float fTimeDelta);
	
private:	
	CTransform*		m_pTransform = { nullptr };
	CTransform*		m_pPlayerTransform = { nullptr };
	CMovement*		m_pMovement = { nullptr };

private:
	_bool	m_bIsAnimChanged{ false };
	_float	m_fCurActionDelay{ 0.f };
	_float	m_fMaxActionDelay{ 1.f };

public:
	static CBowman_State_SnapAttack* Create(void* pArg);
	virtual void Free() override;
};
END	