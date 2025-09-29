#include "pch.h"
#include "Gumpounder_StateMachine.h"
#include "GameInstance.h"

//States
#include "Gumpounder_State_Idle.h"
#include "Gumpounder_State_Aggroed.h"
#include "Gumpounder_State_Death.h"
#include "Gumpounder_State_Hooked.h"
#include "Gumpounder_State_Patrol.h"
#include "Gumpounder_State_AttackCross.h"
#include "Gumpounder_State_AttackCross2.h"
#include "Gumpounder_State_AttackOverhead.h"
#include "Gumpounder_State_GrabSweep.h"
#include "Gumpounder_State_Combo.h"
#include "Gumpounder_State_Capsize.h"
#include "Gumpounder_State_Stagger.h"
#include "Gumpounder_State_Walk.h"

CGumpounder_StateMachine::CGumpounder_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CGumpounder_StateMachine::CGumpounder_StateMachine(const CGumpounder_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CGumpounder_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CGumpounder_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;
	
	m_pDefaultState = CGumpounder_State_Idle::Create(m_pOwner);

	//Add_State((_uint)STATE_UNAGGRO, L"Wait", m_pDefaultState);

	//Add_State((_uint)STATE_CLOSE, L"Combo_Attack", CGumpounder_State_Combo::Create(m_pOwner));
	//Add_State((_uint)STATE_CLOSE, L"Overhead_Attack", CGumpounder_State_AttackOverhead::Create(m_pOwner));
	//Add_State((_uint)STATE_CLOSE, L"Cross_Attack", CGumpounder_State_AttackCross::Create(m_pOwner));
	//Add_State((_uint)STATE_CLOSE, L"Cross2_Attack", CGumpounder_State_AttackCross2::Create(m_pOwner));
	//Add_State((_uint)STATE_CLOSE, L"Grab_Attack", CGumpounder_State_GrabSweep::Create(m_pOwner));

	//Add_State((_uint)STATE_RANGED, L"Chase", CGumpounder_State_Walk::Create(m_pOwner));	

	Add_State((_uint)STATE_OTHER, L"Patrol", CGumpounder_State_Patrol::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Death", CGumpounder_State_Death::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Capsized", CGumpounder_State_Idle::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Stagger", CGumpounder_State_Stagger::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Aggroed", CGumpounder_State_Aggroed::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Hooked", CGumpounder_State_Hooked::Create(m_pOwner));

	Add_State((_uint)STATE_UNAGGRO, L"Wait", m_pDefaultState);
	Add_State((_uint)STATE_CLOSE, L"Wait", m_pDefaultState);	
	Add_State((_uint)STATE_RANGED, L"Wait", m_pDefaultState);

	return S_OK;
}

void CGumpounder_StateMachine::Priority_Update(void* pObj, _float fTimeDelta)
{
	super::Priority_Update(pObj, fTimeDelta);
}

void CGumpounder_StateMachine::Update(void* pObj, _float fTimeDelta)
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

CGumpounder_StateMachine* CGumpounder_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGumpounder_StateMachine* pInstance = new CGumpounder_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CGumpounder_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CGumpounder_StateMachine::Clone(void* pArg)
{
	CGumpounder_StateMachine* pInstance = new CGumpounder_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGumpounder_StateMachine::Free()
{
	__super::Free();
}

