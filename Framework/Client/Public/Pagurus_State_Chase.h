#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
class CFSM;
END

BEGIN(Client)

class CBoss_Pagurus;
class CPagurus_State_Chase final : public CState
{
	using super = CState;

private:
	CPagurus_State_Chase();
	virtual ~CPagurus_State_Chase() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Call_SandBurst(_float fTimeDelta);

private:
	CBoss_Pagurus*	m_pPagurus{ nullptr };
	CMovement*		m_pMovement{ nullptr };
	CTransform*		m_pPlayerTransform{ nullptr };
	CTransform*		m_pTransform{ nullptr };
	CFSM*			m_pFSM{ nullptr };

private:
	const _float4x4* m_pWorldMatrix{ nullptr };
	_bool			m_bLeftTurn{};
	_float			m_fSandBurstTime{};
	_float			m_fSandBurstTimeAcc{};

public:
	static CPagurus_State_Chase* Create(void* pArg);
	virtual void Free() override;
};
END