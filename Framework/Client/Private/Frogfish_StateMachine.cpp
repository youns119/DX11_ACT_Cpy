#include "pch.h"
#include "Frogfish_StateMachine.h"
#include "GameInstance.h"

//States

CFrogfish_StateMachine::CFrogfish_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CFrogfish_StateMachine::CFrogfish_StateMachine(const CFrogfish_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CFrogfish_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CFrogfish_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;
	
/*	m_pDefaultState = CSardine_State_Wait::Create(m_pOwner);

	Add_State((_uint)STATE_UNAGGRO, L"Wait", CSardine_State_Wait::Create(m_pOwner));
	Add_State((_uint)STATE_CLOSE, L"Spin_Attack", CSardine_State_SpinAttack::Create(m_pOwner));
	Add_State((_uint)STATE_CLOSE, L"Back_Strafe", CSardine_State_Strafe::Create(m_pOwner));
	Add_State((_uint)STATE_RANGED, L"Charge_Attack", CSardine_State_ChargeAttack::Create(m_pOwner));
	Add_State((_uint)STATE_RANGED, L"Side_Strafe", CSardine_State_StrafeSide::Create(m_pOwner));
	Add_State((_uint)STATE_RANGED, L"Wait", CSardine_State_Wait::Create(m_pOwner));
	Add_State((_uint)STATE_FAR, L"Chase", CSardine_State_Chase::Create(m_pOwner));
	Add_State((_uint)STATE_OUTOFRANGE, L"Retreat", CSardine_State_Retreat::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Patrol", CSardine_State_Patrol::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Death", CSardine_State_Death::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Capsized", CSardine_State_Capsize::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Stagger", CSardine_State_Stagger::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Aggroed", CSardine_State_Aggroed::Create(m_pOwner))*/;

	return S_OK;
}

void CFrogfish_StateMachine::Priority_Update(void* pObj, _float fTimeDelta)
{
	super::Priority_Update(pObj, fTimeDelta);
}

void CFrogfish_StateMachine::Update(void* pObj, _float fTimeDelta)
{
	super::Update(pObj, fTimeDelta);

#ifdef _DEBUG

	//ImGui::Begin("State Debugger");

	//ImGui::NewLine();
	//
	//ImGui::Text("State Group: %s", m_Mode[m_eMachineMode]);
	//ImGui::Text("State Group: %s", m_Enums[m_iCurGroup]);
	//ImGui::Text("Priority Level: %d", m_pCurrentState->Get_PriorLevel());
	//ImGui::Text("Current State : %s", m_pCurrentState->Get_CurrentStateName());
	////ImGui::Text("Current CoolTime / Max CoolTime : %f / %f", m_pCurrentState->Get_CurCoolTime(), m_pCurrentState->Get_MaxCoolTime());

	//for (auto& item : m_StateGroups[m_iCurGroup])
	//{
	//	ImGui::NewLine();
	//	ImGui::Text("Priority Level: %d", item.second->Get_PriorLevel());
	//	ImGui::Text("Current State : %s", item.second->Get_CurrentStateName());
	//	ImGui::Text("Current CoolTime / Max CoolTime : %f / %f", item.second->Get_CurCoolTime(), item.second->Get_MaxCoolTime());
	//}

	//ImGui::End();
#endif 
}

CFrogfish_StateMachine* CFrogfish_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFrogfish_StateMachine* pInstance = new CFrogfish_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFrogfish_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CFrogfish_StateMachine::Clone(void* pArg)
{
	CFrogfish_StateMachine* pInstance = new CFrogfish_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrogfish_StateMachine::Free()
{
	__super::Free();
}
