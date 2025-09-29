#pragma once

#include "Client_Defines.h"
#include "FSM.h"

BEGIN(Client)

class CInkerton_StateMachine final : public CFSM
{
	using super = CFSM;

public:
	struct IMKERTON_FSM_DESC
	{
		CFSM::FSM_DESC pFSMDesc;		
	};

public:
	enum STATE_TYPE 
	{ 		
		STATE_CLOSE,
		STATE_RANGED,
		STATE_FAR, 
		STATE_OUTOFRANGE,
		STATE_OTHER,
		STATE_END 
	};

private:
	CInkerton_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInkerton_StateMachine(const CInkerton_StateMachine& Prototype);	
	virtual ~CInkerton_StateMachine() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(void* pObj, _float ftimedelta) override;
	virtual void Update(void* pObj, _float ftimedelta) override;

private:
	HRESULT Ready_States();

#ifdef _DEBUG
	vector<const char*> m_Enums{		
		"STATE_CLOSE",
		"STATE_RANGED",
		"STATE_FAR",
		"STATE_OUTOFRANGE",
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
	static CInkerton_StateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END