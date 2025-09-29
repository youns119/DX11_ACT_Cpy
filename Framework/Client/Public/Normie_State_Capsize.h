#pragma once
#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CEnemy_Normie;

class CNormie_State_Capsize final : public CState
{
	using super = CState;

private:
	CNormie_State_Capsize();
	virtual ~CNormie_State_Capsize() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_bool Decrease_CapsizedDuration(_float fTimeDelta);

private:
	CEnemy_Normie* m_pNormie{ nullptr };

private:
	_float m_fCapsizeTime{ 0.f };
	_float m_fCapsizeDuration{ 1.5f };

public:
	static CNormie_State_Capsize* Create(void* pArg);
	virtual void Free() override;
};
END	
