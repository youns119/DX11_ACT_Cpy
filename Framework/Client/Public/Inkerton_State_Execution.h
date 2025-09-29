#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CInkerton_State_Execution final : public CState
{
	using super = CState;

private:
	CInkerton_State_Execution();
	virtual ~CInkerton_State_Execution() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;	

private:
	CBoss_Inkerton*		m_pInkerton{ nullptr };

private:
	_int				m_iAnimIndex{ 0 };
	_float				m_fCurActionDelay{ 0.f };
	_float				m_fMaxActionDelay{ 1.f };

public:
	static CInkerton_State_Execution* Create(void* pObj);
	virtual void Free() override;
};
END	