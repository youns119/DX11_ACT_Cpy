#include "pch.h"
#include "Roland_StateMachine.h"
#include "GameInstance.h"

//States
#include "Roland_State_MultiThrustAttack.h"
#include "Roland_State_BigCombo.h"
#include "Roland_State_Chase.h"
#include "Roland_State_GolfAttack.h"
#include "Roland_State_Dodge.h"
#include "Roland_State_Capsize.h"
#include "Roland_State_SideWalk.h"
#include "Roland_State_Hooked.h"
#include "Roland_State_Walk.h"
#include "Roland_State_SingleThrustAttack.h"
#include "Roland_State_VaudGrab.h"
#include "Roland_State_Unroll.h"
#include "Roland_State_Roll.h"
#include "Roland_State_Wallop.h"
#include "Roland_State_Stagger.h"
#include "Roland_State_OffhandPunch.h"
#include "Roland_State_XSlashAttack.h"
#include "Roland_State_Wait.h"
#include "Roland_State_Death.h"
#include "Roland_State_Run.h"
#include "Roland_State_Aggroed.h"

CRoland_StateMachine::CRoland_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CRoland_StateMachine::CRoland_StateMachine(const CRoland_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CRoland_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CRoland_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;
	
	m_pDefaultState = CRoland_State_Wait::Create(m_pOwner);
	
	/* Close */
	{
		Add_State((_uint)STATE_CLOSE, L"Multi_Thrust", CRoland_State_MultiThrustAttack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Single_Thrust", CRoland_State_SingleThrustAttack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Golf_Attack", CRoland_State_GolfAttack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Wallop", CRoland_State_Wallop::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Big_Combo", CRoland_State_BigCombo::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"XSlash_Attack", CRoland_State_XSlashAttack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"OffhandPunch", CRoland_State_OffhandPunch::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Dodge", CRoland_State_Dodge::Create(m_pOwner));
	}

	/* Ranged */
	{
		Add_State((_uint)STATE_RANGED, L"Roll", CRoland_State_Roll::Create(m_pOwner));
		Add_State((_uint)STATE_RANGED, L"Side_Walk", CRoland_State_SideWalk::Create(m_pOwner));
		Add_State((_uint)STATE_RANGED, L"Walk", CRoland_State_Walk::Create(m_pOwner));
	}

	/* Far */
	{
		Add_State((_uint)STATE_OUTOFRANGE, L"Run", CRoland_State_Run::Create(m_pOwner));
	}

	/* Other */
	{
		Add_State((_uint)STATE_OTHER, L"Wait", m_pDefaultState);
		Add_State((_uint)STATE_OTHER, L"Death", CRoland_State_Death::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Capsized", CRoland_State_Capsize::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Stagger", CRoland_State_Stagger::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Aggroed", CRoland_State_Aggroed::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Unroll", CRoland_State_Unroll::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Hooked", CRoland_State_Hooked::Create(m_pOwner));		
	}

	return S_OK;
}

void CRoland_StateMachine::Priority_Update(void* pObj, _float ftimedelta)
{
	super::Priority_Update(pObj, ftimedelta);
}

void CRoland_StateMachine::Update(void* pObj, _float ftimedelta)
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

CRoland_StateMachine* CRoland_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoland_StateMachine* pInstance = new CRoland_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CRoland_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CRoland_StateMachine::Clone(void* pArg)
{
	CRoland_StateMachine* pInstance = new CRoland_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_StateMachine::Free()
{
	__super::Free();
}

