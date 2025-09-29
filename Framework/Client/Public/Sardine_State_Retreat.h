#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)

class CSardine_State_Retreat final : public CState
{
	using super = CState;

private:
	CSardine_State_Retreat();
	virtual ~CSardine_State_Retreat() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:	
	CMovement*		m_pMovement = { nullptr };	
	CTransform*		m_pTransform = { nullptr };
	CTransform*		m_pPlayerTransform = { nullptr };

	_vec3			m_vOriginalPos{};
	
public:
	static CSardine_State_Retreat* Create(void* pArg);
	virtual void Free() override;
};
END	