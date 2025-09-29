#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "EnemyStateEnum.h"

BEGIN(Client)
class CEnemy_Cuttlefish;

class CCuttlefish_State_Move final : public CState
{
	using super = CState;

private:
	CCuttlefish_State_Move();
	virtual ~CCuttlefish_State_Move() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Decrease_Duration(_float fTimeDelta);
	void Select_Action();

private:
	CEnemy_Cuttlefish*	m_pCuttlefish{ nullptr };
	CTransform*			m_pTransform{ nullptr };
	CTransform*			m_pPlayerTransform{ nullptr };

private:
	_int				m_iAnimIndex{ 0 };
	_float				m_fCurActDuration{ 0.f };
	_float				m_fCurMoveDuration{ 0.f };
	_float				m_fActDuration{ 0.f };
	_float				m_fMoveDuration{ 5.f };

public:
	static CCuttlefish_State_Move* Create(void* pArg);
	virtual void Free() override;
};
END	