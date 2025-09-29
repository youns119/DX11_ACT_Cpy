#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CCuttlefish_State_ThrowProjectile final : public CState
{
	using super = CState;

private:
	CCuttlefish_State_ThrowProjectile();
	virtual ~CCuttlefish_State_ThrowProjectile() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_bool After_AttackDelay(_float fTimeDelta);

private:	
	CMovement*		m_pMovement = { nullptr };
	CTransform*		m_pPlayerTransform = { nullptr };

private:
	_int			m_iCurAnimIndex{ 0 };

private:
	_bool			m_bIsAnimChanged{ false };
	_float			m_fCurActionDelay{ 0.f };
	_float			m_fMaxActionDelay{ 1.f };
	
public:
	static CCuttlefish_State_ThrowProjectile* Create(void* pArg);
	virtual void Free() override;
};
END	