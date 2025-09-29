#pragma once

#include "Client_Defines.h"
#include "State.h"
#include "Boss_Roland.h"

BEGIN(Engine)
class CMovement;
class CTransform;
END

BEGIN(Client)

class CRoland_State_Roll final : public CState
{
	using super = CState;
	

private:
	CRoland_State_Roll();
	virtual ~CRoland_State_Roll() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Set_BellyFlopSpeed();
	void Belly_Flop(_float fTimeDelta);
	void Roll(_float fTimeDelta);
	_bool PreRoll(_float fTimeDelta);
	_float Compute_DirectDistance(_vec2 vA, _vec2 vB);

	_float Compute_Distance(_vec2 vA, _vec2 vB);

	void Call_Bellyflop_Effect();

private:
	CBoss_Roland*	m_pRoland{ nullptr };
	CTransform*		m_pTransform{ nullptr };
	CTransform*		m_pPlayerTransform{ nullptr };
	CMovement*		m_pMovement{ nullptr };	

private:
	_vec3			m_vRollTargetPos{};
	_vec3			m_vFlopPos{};

private:
	_int			m_iCurAnimIndex{ 0 };
	_int			m_bIsFirstBounce{ true };
	_bool			m_bIsFlop{ false };	
	_float			m_fFlopSpeed{ 0.f };
	_float			m_fCurPreRollTime{ 0.f };
	_float			m_fMaxPreRollTime{ 1.5f };

public:
	static CRoland_State_Roll* Create(void* pArg);
	virtual void Free() override;
};
END	