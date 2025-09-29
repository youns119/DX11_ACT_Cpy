#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CBoss_Petroch;

class CPetroch_State_Hooked final : public CState
{
	using super = CState;

private:
	CPetroch_State_Hooked();
	virtual ~CPetroch_State_Hooked() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	CBoss_Petroch* m_pPetroch{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

private:
	_int	m_iAnimIndex{ 0 };

public:
	static CPetroch_State_Hooked* Create(void* pArg);
	virtual void Free() override;
};
END	