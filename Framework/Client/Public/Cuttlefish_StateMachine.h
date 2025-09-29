#pragma once

#include "Client_Defines.h"
#include "FSM.h"

BEGIN(Client)

class CCuttlefish_StateMachine final : public CFSM
{
	using super = CFSM;

public:
	enum STATE_TYPE 
	{ 
		STATE_UNAGGRO,
		STATE_CLOSE,
		STATE_RANGED,
		STATE_FAR, 		
		STATE_OTHER,
		STATE_END 
	};

private:
	CCuttlefish_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCuttlefish_StateMachine(const CCuttlefish_StateMachine& Prototype);	
	virtual ~CCuttlefish_StateMachine() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(void* pObj, _float ftimedelta) override;
	virtual void Update(void* pObj, _float ftimedelta) override;

#ifdef _DEBUG
	vector<const char*> m_Enums{
		"STATE_UNAGGRO",
		"STATE_CLOSE",
		"STATE_RANGED",
		"STATE_FAR",		
		"STATE_OTHER",
		"STATE_END"
	};

	vector<const char*> m_Mode{
	"FSM_PRIOR",
	"FSM_RAND",
	"FSM_END",
	};
#endif

	
public:
	static CCuttlefish_StateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END