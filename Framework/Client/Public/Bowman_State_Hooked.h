#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CEnemy_Bowman;

class CBowman_State_Hooked final : public CState
{
	using super = CState;

private:
	CBowman_State_Hooked();
	virtual ~CBowman_State_Hooked() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	CEnemy_Bowman* m_pBowman{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

public:
	static CBowman_State_Hooked* Create(void* pArg);
	virtual void Free() override;
};
END	