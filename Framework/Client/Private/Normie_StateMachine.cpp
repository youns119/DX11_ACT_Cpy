#include "pch.h"
#include "Normie_StateMachine.h"
#include "GameInstance.h"

//States
#include "Normie_State_Aggroed.h"
#include "Normie_State_Death.h"
#include "Normie_State_Hooked.h"
#include "Normie_State_Idle.h"
#include "Normie_State_Patrol.h"
#include "Normie_State_SlamAttack.h"
#include "Normie_State_SmashAttack.h"
#include "Normie_State_Stagger.h"
#include "Normie_State_Walk.h"
#include "Normie_State_Capsize.h"

CNormie_StateMachine::CNormie_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CNormie_StateMachine::CNormie_StateMachine(const CNormie_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CNormie_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CNormie_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;
	
	m_pDefaultState = CNormie_State_Idle::Create(m_pOwner);

	Add_State((_uint)STATE_UNAGGRO, L"Wait", m_pDefaultState);

	Add_State((_uint)STATE_CLOSE, L"Slam_Attack", CNormie_State_SlamAttack::Create(m_pOwner));
	Add_State((_uint)STATE_CLOSE, L"Smash_Attack", CNormie_State_SmashAttack::Create(m_pOwner));

	Add_State((_uint)STATE_OUTOFRANGE, L"Chase", CNormie_State_Walk::Create(m_pOwner));	

	Add_State((_uint)STATE_OTHER, L"Patrol", CNormie_State_Patrol::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Death", CNormie_State_Death::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Capsized", CNormie_State_Capsize::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Stagger", CNormie_State_Stagger::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Aggroed", CNormie_State_Aggroed::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Hooked", CNormie_State_Hooked::Create(m_pOwner));

	//Add_State((_uint)STATE_UNAGGRO, L"Wait", m_pDefaultState);
	//Add_State((_uint)STATE_CLOSE, L"Wait", m_pDefaultState);
	//Add_State((_uint)STATE_FAR, L"Wait", m_pDefaultState);
	//Add_State((_uint)STATE_RANGED, L"Wait", m_pDefaultState);

	return S_OK;
}

void CNormie_StateMachine::Priority_Update(void* pObj, _float fTimeDelta)
{
	super::Priority_Update(pObj, fTimeDelta);
}

void CNormie_StateMachine::Update(void* pObj, _float fTimeDelta)
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

CNormie_StateMachine* CNormie_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNormie_StateMachine* pInstance = new CNormie_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNormie_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CNormie_StateMachine::Clone(void* pArg)
{
	CNormie_StateMachine* pInstance = new CNormie_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNormie_StateMachine::Free()
{
	__super::Free();
}
