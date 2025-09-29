#include "pch.h"
#include "Pagurus_StateMachine.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"

//States
#include "Pagurus_State_Execution.h"
#include "Pagurus_State_Wait.h"
#include "Pagurus_State_Patrol.h"
#include "Pagurus_State_Chase.h"


CPagurus_StateMachine::CPagurus_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPagurus_StateMachine::CPagurus_StateMachine(const CPagurus_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CPagurus_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CPagurus_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;

	IMKERTON_FSM_DESC* pDesc = static_cast<IMKERTON_FSM_DESC*>(pArg);

	if (FAILED(Ready_States()))
		return E_FAIL;

	return S_OK;
}

void CPagurus_StateMachine::Priority_Update(void* pObj, _float ftimedelta)
{
	super::Priority_Update(pObj, ftimedelta);
}

void CPagurus_StateMachine::Update(void* pObj, _float ftimedelta)
{
	super::Update(pObj, ftimedelta);

#ifdef _DEBUG

	//ImGui::Begin("State Debugger");

	//ImGui::NewLine();
	//
	//ImGui::Text("Pattern Mode: %s", m_Mode[m_eMachineMode]);
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

HRESULT CPagurus_StateMachine::Ready_States()
{
	m_pDefaultState = CPagurus_State_Wait::Create(m_pOwner);	

	/* Close */
	{
		Add_State((_uint)STATE_CLOSE, L"Execution", CPagurus_State_Execution::Create(m_pOwner));
	}

	Add_State((_uint)STATE_OTHER, L"Chase", CPagurus_State_Chase::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Patrol", CPagurus_State_Patrol::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Wait", m_pDefaultState);
	return S_OK;
}

CPagurus_StateMachine* CPagurus_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPagurus_StateMachine* pInstance = new CPagurus_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPagurus_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPagurus_StateMachine::Clone(void* pArg)
{
	CPagurus_StateMachine* pInstance = new CPagurus_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPagurus_StateMachine::Free()
{
	__super::Free();
}

