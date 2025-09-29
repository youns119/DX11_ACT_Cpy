#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CInkerton_State_Retreat final : public CState
{
	using super = CState;

private:
	CInkerton_State_Retreat();
	virtual ~CInkerton_State_Retreat() = default;

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
	_float3			m_fSacle{};
	_float			m_fScaleDecreaseAmount{ 0.06f };
	_float			m_fCurRetreatTime{ 0.f };
	_float			m_fMaxRetreatTime{ .3f };
	
public:
	static CInkerton_State_Retreat* Create(void* pArg);
	virtual void Free() override;
};
END	