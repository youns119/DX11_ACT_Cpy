#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CEnemy_Splitter;

class CSplitter_State_Capsize final : public CState
{
	using super = CState;

private:
	CSplitter_State_Capsize();
	virtual ~CSplitter_State_Capsize() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_bool Decrease_CapsizedDuration(_float fTimeDelta);
	
private:
	CEnemy_Splitter* m_pSplitter{ nullptr };

private:
	_int m_iAnimIndex = { 0 };

private:
	_float m_fCapsizeTime{ 0.f };
	_float m_fCapsizeDuration{ 1.5f };

public:
	static CSplitter_State_Capsize* Create(void* pObj);
	virtual void Free() override;
};
END	