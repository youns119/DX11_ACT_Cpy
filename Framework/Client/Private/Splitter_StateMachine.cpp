#include "pch.h"
#include "Splitter_StateMachine.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

//States
#include "Splitter_State_Run.h"
#include "Splitter_State_Fear.h"
#include "Splitter_State_Wait.h"
#include "Splitter_State_Walk.h"
#include "Splitter_State_Death.h"
#include "Splitter_State_Patrol.h"
#include "Splitter_State_Aggroed.h"
#include "Splitter_State_Capsize.h"
#include "Splitter_State_Stagger.h"
#include "Splitter_State_Retreat.h"
#include "Splitter_State_BashAttack.h"
#include "Splitter_State_SwipeAttack.h"
#include "Splitter_State_GuillotineHit.h"
#include "Splitter_State_GuillotineSlam.h"
#include "Splitter_State_ExecutionSwipeAttack.h"

CSplitter_StateMachine::CSplitter_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CSplitter_StateMachine::CSplitter_StateMachine(const CSplitter_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CSplitter_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSplitter_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;

	m_pDefaultState = CSplitter_State_Wait::Create(m_pOwner);

	CState* pStateFear = CSplitter_State_Fear::Create(m_pOwner);
	CState* pStateWalk = CSplitter_State_Walk::Create(m_pOwner);
	CState* pStateSwipeAttack = CSplitter_State_SwipeAttack::Create(m_pOwner);
	CState* pStateExecutionSwipeAttack = CSplitter_State_ExecutionSwipeAttack::Create(m_pOwner);

	Add_State((_uint)STATE_UNAGGRO, L"Wait", m_pDefaultState);

	/* Cloase */
	{
		Add_State((_uint)STATE_CLOSE, L"Bash_Attack", CSplitter_State_BashAttack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Guillo_Hit", CSplitter_State_GuillotineHit::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Swipe_Attack", pStateSwipeAttack);
		Add_State((_uint)STATE_CLOSE, L"Execution_Swipe_Attack", pStateExecutionSwipeAttack);
		Add_State((_uint)STATE_CLOSE, L"Fear", pStateFear);
	}

	/* Ranged */
	{
		Add_State((_uint)STATE_RANGED, L"Swipe_Attack", pStateSwipeAttack);
		Add_State((_uint)STATE_RANGED, L"Fear", pStateFear);		
		Add_State((_uint)STATE_RANGED, L"Guillo_Slam", CSplitter_State_GuillotineSlam::Create(m_pOwner));
		Add_State((_uint)STATE_RANGED, L"Execution_Swipe_Attack", pStateExecutionSwipeAttack);		
		Add_State((_uint)STATE_RANGED, L"Walk", pStateWalk);	
	}

	/* Far */
	{
		Add_State((_uint)STATE_FAR, L"Walk", pStateWalk);
	}

	/* Out of Range */
	{
		Add_State((_uint)STATE_OUTOFRANGE, L"Run", CSplitter_State_Run::Create(m_pOwner));
	}
	
	/* Other */
	{
		Add_State((_uint)STATE_OTHER, L"Patrol", CSplitter_State_Patrol::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Death", CSplitter_State_Death::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Capsized", CSplitter_State_Capsize::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Stagger", CSplitter_State_Stagger::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Aggroed", CSplitter_State_Aggroed::Create(m_pOwner));
	}

	return S_OK;
}

void CSplitter_StateMachine::Priority_Update(void* pObj, _float ftimedelta)
{
	super::Priority_Update(pObj, ftimedelta);
}

void CSplitter_StateMachine::Update(void* pObj, _float ftimedelta)
{
	super::Update(pObj, ftimedelta);

#ifdef _DEBUG

	//ImGui::Begin("State Debugger");

	//ImGui::NewLine();

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

CSplitter_StateMachine* CSplitter_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSplitter_StateMachine* pInstance = new CSplitter_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSplitter_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSplitter_StateMachine::Clone(void* pArg)
{
	CSplitter_StateMachine* pInstance = new CSplitter_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_StateMachine::Free()
{
	__super::Free();
}
