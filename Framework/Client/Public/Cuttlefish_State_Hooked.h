#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CEnemy_Cuttlefish;

class CCuttlefish_State_Hooked final : public CState
{
	using super = CState;

private:
	CCuttlefish_State_Hooked();
	virtual ~CCuttlefish_State_Hooked() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Reset_RadialblurParams();

private:	
	CEnemy_Cuttlefish*	m_pCuttlefish{ nullptr };
	CTransform*			m_pTransform{ nullptr };
	CTransform*			m_pPlayerTransform{ nullptr };
	
public:
	static CCuttlefish_State_Hooked* Create(void* pArg);
	virtual void Free() override;
};
END	