#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CInkerton_State_DodgeRightAttack final : public CState
{
	using super = CState;

private:
	CInkerton_State_DodgeRightAttack();
	virtual ~CInkerton_State_DodgeRightAttack() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:		
	CTransform*		m_pTransform{ nullptr };

private:
	_int			m_iAnimIndex{ 0 };
	
public:
	static CInkerton_State_DodgeRightAttack* Create(void* pArg);
	virtual void Free() override;
};
END	