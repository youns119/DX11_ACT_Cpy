#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CGumpounder_State_Patrol final : public CState
{
	using super = CState;

private:
	CGumpounder_State_Patrol();
	virtual ~CGumpounder_State_Patrol() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:	
	CMovement*			m_pMovement{ nullptr };	
	CTransform*			m_pTransform{ nullptr };
	CTransform*			m_pPlayerTransform{ nullptr };

private:
	_int				m_iNextPosIndex{ 0 };
	_vec3				m_vTargetPos{};
	vector<_vec3>		m_pParolPoints{};
	
public:
	static CGumpounder_State_Patrol* Create(void* pArg);
	virtual void Free() override;
};
END		