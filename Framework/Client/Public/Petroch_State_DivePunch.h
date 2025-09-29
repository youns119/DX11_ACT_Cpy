#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CBoss_Petroch;

class CPetroch_State_DivePunch final : public CState
{
	using super = CState;

private:
	CPetroch_State_DivePunch();
	virtual ~CPetroch_State_DivePunch() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;	

private:
	void Dive(_float fTimeDelta, _float t);
	void Compute_DiveSpeed();
	_float Compute_Distance(_vector vA, _vector vB);	

private:
	CBoss_Petroch*	m_pPetroch{ nullptr };
	CTransform*		m_pTransform{ nullptr };
	CTransform*		m_pPlayerTransform{ nullptr };

private:
	_bool			m_bIsDive{ false };
	_int			m_iAnimIndex{ 0 };
	_float			m_fInterporateTime{ 0.f };
	_float			m_fDive_Speed{ 0.f };
	_float4			m_vDivePos{};

public:
	static CPetroch_State_DivePunch* Create(void* pArg);
	virtual void Free() override;
};
END	