#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CPlayer;
class CEnemy_Splitter;

class CSplitter_State_Fear final : public CState
{
	using super = CState;

private:
	CSplitter_State_Fear();
	virtual ~CSplitter_State_Fear() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	_bool After_AttackDelay(_float fTimeDelta);

private:	
	CEnemy_Splitter*	m_pSplitter{ nullptr };
	CPlayer*			m_pPlayer{ nullptr };
	CTransform*			m_pPlayerTransform{ nullptr };
	CTransform*			m_pTransform{ nullptr };

private:
	_bool				m_bIsAnimChanged{ false };
	_float				m_fCurActionDelay{ 0.f };
	_float				m_fMaxActionDelay{ 1.f };

public:
	static CSplitter_State_Fear* Create(void* pObj);
	virtual void Free() override;
};
END	