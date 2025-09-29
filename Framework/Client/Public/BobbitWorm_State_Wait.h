#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CEnemy_BobbitWorm;

class CBobbitWorm_State_Wait final : public CState
{
	using super = CState;

private:
	CBobbitWorm_State_Wait();
	virtual ~CBobbitWorm_State_Wait() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	_float Compute_Distance(_vec2 vA, _vec2 vB);
	virtual void Exit(void* pObj) override;

private:
	CEnemy_BobbitWorm* m_pBobbitWorm{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

public:
	static CBobbitWorm_State_Wait* Create(void* pArg);
	virtual void Free() override;
};
END