#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CBoss_Inkerton;

class CInkerton_State_Hooked final : public CState
{
	using super = CState;

private:
	CInkerton_State_Hooked();
	virtual ~CInkerton_State_Hooked() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	CBoss_Inkerton* m_pInkerton{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };
	
public:
	static CInkerton_State_Hooked* Create(void* pObj);
	virtual void Free() override;
};
END	