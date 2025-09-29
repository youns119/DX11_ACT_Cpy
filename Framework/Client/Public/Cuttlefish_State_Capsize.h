#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CEnemy_Cuttlefish;

class CCuttlefish_State_Capsize final : public CState
{
	using super = CState;

private:
	CCuttlefish_State_Capsize();
	virtual ~CCuttlefish_State_Capsize() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

	void Reset_RadialblurParams();

private:
	_bool Decrease_CapsizedDuration(_float fTimeDelta);

private:
	CEnemy_Cuttlefish*	m_pCuttlefish{ nullptr };

private:
	_int				m_iCurAnimIndex{ 0 };

private:
	_float				m_fCapsizeTime{ 0.f };
	_float				m_fCapsizeDuration{ 1.5f };

private:
	vector<_wstring>	m_Sounds{};
	
public:
	static CCuttlefish_State_Capsize* Create(void* pArg);
	virtual void Free() override;
};
END	