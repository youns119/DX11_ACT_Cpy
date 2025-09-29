#pragma once
#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CEnemy_Sardine;

class CSardine_State_Capsize final : public CState
{
	using super = CState;

private:
	CSardine_State_Capsize();
	virtual ~CSardine_State_Capsize() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_bool Decrease_CapsizedDuration(_float fTimeDelta);

private:
	CEnemy_Sardine* m_pSardine{ nullptr };

private:
	_float m_fCapsizeTime{ 0.f };
	_float m_fCapsizeDuration{ 1.5f };

public:
	static CSardine_State_Capsize* Create(void* pArg);
	virtual void Free() override;
};
END	
