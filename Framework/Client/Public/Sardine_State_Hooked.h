#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CEnemy_Sardine;

class CSardine_State_Hooked final : public CState
{
	using super = CState;

private:
	CSardine_State_Hooked();
	virtual ~CSardine_State_Hooked() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	CEnemy_Sardine* m_pSardine{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };
public:
	static CSardine_State_Hooked* Create(void* pArg);
	virtual void Free() override;
};
END	