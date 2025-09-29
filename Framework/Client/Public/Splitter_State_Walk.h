#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CModel;
class CMovement;
class CTransform;
END

BEGIN(Client)

class CSplitter_State_Walk final : public CState
{
	using super = CState;

private:
	CSplitter_State_Walk();
	virtual ~CSplitter_State_Walk() = default;

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
	static CSplitter_State_Walk* Create(void* pObj);
	virtual void Free() override;
};
END	