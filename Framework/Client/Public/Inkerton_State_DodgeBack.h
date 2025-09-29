#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CInkerton_State_DodgeBack final : public CState
{
	using super = CState;

private:
	CInkerton_State_DodgeBack();
	virtual ~CInkerton_State_DodgeBack() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:	
	CTransform*		m_pTransform = { nullptr };
	
public:
	static CInkerton_State_DodgeBack* Create(void* pArg);
	virtual void Free() override;
};
END	