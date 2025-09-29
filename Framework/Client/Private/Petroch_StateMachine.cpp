#include "pch.h"
#include "Petroch_StateMachine.h"
#include "GameInstance.h"

//States
#include "Petroch_State_Wait.h"
#include "Petroch_State_Walk.h"
#include "Petroch_State_Block.h"
#include "Petroch_State_BlockWalk.h"
#include "Petroch_State_Capsize.h"
#include "Petroch_State_Death.h"
#include "Petroch_State_DivePunch.h"
#include "Petroch_State_GroundSlam.h"
#include "Petroch_State_Hide.h"
#include "Petroch_State_Pounce.h"
#include "Petroch_State_Hooked.h"
#include "Petroch_State_Retreat.h"
#include "Petroch_State_Roll.h"
#include "Petroch_State_Stagger.h"


CPetroch_StateMachine::CPetroch_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPetroch_StateMachine::CPetroch_StateMachine(const CPetroch_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CPetroch_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CPetroch_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;
	
	m_pDefaultState = CPetroch_State_Wait::Create(m_pOwner);

	CState* pWalk= CPetroch_State_Walk::Create(m_pOwner);
	CState* pPounce = CPetroch_State_Pounce::Create(m_pOwner);
	CState* pGroundSlam = CPetroch_State_GoundSlam::Create(m_pOwner);
	CState* pDivePunch = CPetroch_State_DivePunch::Create(m_pOwner);
	
	Add_State((_uint)STATE_CLOSE, L"Walk", pWalk);
	Add_State((_uint)STATE_CLOSE, L"Pounce", pPounce);
	Add_State((_uint)STATE_CLOSE, L"Block", CPetroch_State_Block::Create(m_pOwner));	
	Add_State((_uint)STATE_CLOSE, L"Retreat", CPetroch_State_Retreat::Create(m_pOwner));
	Add_State((_uint)STATE_CLOSE, L"DivePunch", pDivePunch);

	Add_State((_uint)STATE_RANGED, L"Walk", pWalk);
	Add_State((_uint)STATE_RANGED, L"Pounce", pPounce);
	Add_State((_uint)STATE_RANGED, L"GroundSlam", pGroundSlam);
	Add_State((_uint)STATE_RANGED, L"DivePunch", pDivePunch);
	Add_State((_uint)STATE_RANGED, L"Roll", CPetroch_State_Roll::Create(m_pOwner));

	Add_State((_uint)STATE_OUTOFRANGE, L"Walk", pWalk);
	Add_State((_uint)STATE_OUTOFRANGE, L"GroundSlam", pGroundSlam);
	Add_State((_uint)STATE_OUTOFRANGE, L"Roll", CPetroch_State_Roll::Create(m_pOwner));

	Add_State((_uint)STATE_OTHER, L"Wait", m_pDefaultState);
	Add_State((_uint)STATE_OTHER, L"Hide", CPetroch_State_Hide::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Stagger", CPetroch_State_Stagger::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Capsize", CPetroch_State_Capsize::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Death", CPetroch_State_Death::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Hooked", CPetroch_State_Hooked::Create(m_pOwner));

	return S_OK;
}

void CPetroch_StateMachine::Priority_Update(void* pObj, _float ftimedelta)
{
	super::Priority_Update(pObj, ftimedelta);
}

void CPetroch_StateMachine::Update(void* pObj, _float ftimedelta)
{
	super::Update(pObj, ftimedelta);

#ifdef _DEBUG

	/*ImGui::Begin("State Debugger");

	ImGui::NewLine();
	*/
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

CPetroch_StateMachine* CPetroch_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPetroch_StateMachine* pInstance = new CPetroch_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPetroch_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPetroch_StateMachine::Clone(void* pArg)
{
	CPetroch_StateMachine* pInstance = new CPetroch_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_StateMachine::Free()
{
	__super::Free();
}
