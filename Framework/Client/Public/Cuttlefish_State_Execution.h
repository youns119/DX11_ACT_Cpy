#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CEnemy_Cuttlefish;

class CCuttlefish_State_Execution final : public CState
{
	using super = CState;

private:
	CCuttlefish_State_Execution();
	virtual ~CCuttlefish_State_Execution() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Reset_RadialblurParams();
	_bool After_AttackDelay(_float fTimeDelta);
	_bool Hypnosis_Delay(_float fTimeDelta);

private:	
	CEnemy_Cuttlefish*	m_pCuttlefish{ nullptr };
	CMovement*			m_pMovement = { nullptr };
	CTransform*			m_pPlayerTransform = { nullptr };	

private:	
	_bool				m_bIsAnimChanged{ false };
	_int				m_iCurAnimIndex{ -1 };
	_float				m_fCurActionDelay{ 0.f };
	_float				m_fMaxActionDelay{ 1.f };
	_float				m_fMaxHypnosisDelay{ 1.f };

private:
	_float				m_fCurIntensity{ 1.f };
	_float				m_fCurDensity{ 1.f };
	_float				m_fTargetDensity{ 4.f };
	_float				m_fTargetIntensity{2.f};	

	
public:
	static CCuttlefish_State_Execution* Create(void* pArg);
	virtual void Free() override;
};
END	