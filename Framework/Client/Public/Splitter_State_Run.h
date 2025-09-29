#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CModel;
class CMovement;
class CTransform;
END

BEGIN(Client)

class CSplitter_State_Run final : public CState
{
	using super = CState;

private:
	CSplitter_State_Run();
	virtual ~CSplitter_State_Run() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;
	
private:	
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr }; 

public:
	static CSplitter_State_Run* Create(void* pObj);
	virtual void Free() override;
};
END	