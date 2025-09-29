#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CEnemy_Splitter;

class CSplitter_State_Hooked final : public CState
{
	using super = CState;

private:
	CSplitter_State_Hooked();
	virtual ~CSplitter_State_Hooked() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	CEnemy_Splitter* m_pSplitter{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

public:
	static CSplitter_State_Hooked* Create(void* pObjs);
	virtual void Free() override;
};
END	