#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CMovement;
class CTransform;
END

BEGIN(Client)

class CSplitter_State_Retreat final : public CState
{
	using super = CState;

private:
	CSplitter_State_Retreat();
	virtual ~CSplitter_State_Retreat() = default;

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
	static CSplitter_State_Retreat* Create(void* pObj);
	virtual void Free() override;
};
END	