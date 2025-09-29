#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CMovement;
class CTransform;
END

BEGIN(Client)

class CBoss_Petroch;

class CPetroch_State_Roll final : public CState
{
	using super = CState;
	

private:
	CPetroch_State_Roll();
	virtual ~CPetroch_State_Roll() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:	
	_bool Roll(_float fTimeDelta);		

private:
	CBoss_Petroch* m_pPetroch{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };
	CMovement* m_pMovement{ nullptr };

private:
	_int		m_iCurAnimIndex{ 0 };
	_float		m_fCurPreRollTime{ 0.f };
	_float		m_fMaxPreRollTime{ 10.f };

	_float		m_fEffectTimeAcc{};
	_float		m_fEffectTime{0.05f};

private:
	_float4		m_vTargetLook{};

public:
	static CPetroch_State_Roll* Create(void* pArg);
	virtual void Free() override;
};
END	