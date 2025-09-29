#pragma once

#include "Component.h"

BEGIN(Engine)

class CState;
class CGameObject;

class ENGINE_DLL CFSM abstract : public CComponent
{
public:
	enum FSM_MODE { FSM_PRIOR, FSM_RAND, FSM_END };

	enum
	{
		GROUP_END = sizeof(_uint) * 8
	};

public:
	struct FSM_DESC {
		FSM_MODE eMode{ FSM_RAND };
		CGameObject* pOwner{ nullptr };		
	};


protected:
	CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFSM(const CFSM& Prototype);
	virtual ~CFSM() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(void* pObj, _float fTimedelta);
	virtual void Update(void* pObj, _float fTimedelta);

public:
	const _char* Get_CurStateName();

public:
	void Refresh_AllStates();
	void Clear_State();
	void Call_NextState(const _wstring _stateName);

protected:
	void Change_State(void* pObj);
	void Update_State(void* pObj, _float fTimeDelta);
	void Cooldown_States(_float fTimeDelta);	

public:
	void Select_StatesFromGroup(_uint iGroupIndex);

protected:
	void Set_DefaultState(CState* _pState) { m_pDefaultState = _pState; }
	void Add_State(_uint iGoupIndex, const _wstring _szName, CState* _pState);

protected:
	CGameObject* m_pOwner{ nullptr };
	CState* m_pCurrentState{ nullptr };
	CState* m_pDefaultState{ nullptr };
	vector<CState*>							m_NextStates{};
	vector<map<const _wstring, CState*>>	m_StateGroups{};
	_uint									m_iCurGroup{ 0 };

protected:
	FSM_MODE								m_eMachineMode{ FSM_RAND };

public:
	virtual void Free() override;
};

END