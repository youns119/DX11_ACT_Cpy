#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CSardine_State_StrafeSide final : public CState
{
	using super = CState;

private:
	CSardine_State_StrafeSide();
	virtual ~CSardine_State_StrafeSide() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:	
	CTransform*		m_pPlayerTransform{ nullptr };
	CTransform*		m_pTransform{ nullptr };
	CMovement*		m_pMovement{ nullptr };

private:
	_vec3			m_vLookTarget{};	
	_float			m_fStrafeSpeed{ 0.f };
	_float			m_fCurActionDuration{ 0.f };
	_float			m_fMaxActionDuration{ 1.f };
	
public:
	static CSardine_State_StrafeSide* Create(void* pArg);
	virtual void Free() override;
};
END	