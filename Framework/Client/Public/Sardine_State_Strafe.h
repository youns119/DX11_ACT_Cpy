#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CSardine_State_Strafe final : public CState
{
	using super = CState;

private:
	CSardine_State_Strafe();
	virtual ~CSardine_State_Strafe() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:	
	CTransform*		m_pPlayerTransform = { nullptr };
	CTransform*		m_pTransform = { nullptr };
	CMovement*		m_pMovement = { nullptr };

	_vec3			m_vLookTarget = {};
	_float			m_fCurActionDuration = { 0 };
	_float			m_fMaxActionDuration = { 1 };
	
public:
	static CSardine_State_Strafe* Create(void* pArg);
	virtual void Free() override;
};
END	