#include "pch.h"
#include "Sardine_StateMachine.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

//States
#include "Sardine_State_Wait.h"
#include "Sardine_State_Death.h"
#include "Sardine_State_Chase.h"
#include "Sardine_State_Strafe.h"
#include "Sardine_State_Hooked.h"
#include "Sardine_State_Capsize.h"
#include "Sardine_State_Retreat.h"
#include "Sardine_State_Stagger.h"
#include "Sardine_State_Aggroed.h"
#include "Sardine_State_Patrol.h"
#include "Sardine_State_SpinAttack.h"
#include "Sardine_State_StrafeSide.h"
#include "Sardine_State_ChargeAttack.h"

CSardine_StateMachine::CSardine_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CSardine_StateMachine::CSardine_StateMachine(const CSardine_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CSardine_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CSardine_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;
	
	m_pDefaultState = CSardine_State_Wait::Create(m_pOwner);

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
	Add_State((_uint)STATE_OTHER, L"Aggroed", CSardine_State_Aggroed::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Hooked", CSardine_State_Hooked::Create(m_pOwner));

	return S_OK;
}

void CSardine_StateMachine::Priority_Update(void* pObj, _float fTimeDelta)
{
	super::Priority_Update(pObj, fTimeDelta);
}

void CSardine_StateMachine::Update(void* pObj, _float fTimeDelta)
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

CSardine_StateMachine* CSardine_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSardine_StateMachine* pInstance = new CSardine_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSardine_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSardine_StateMachine::Clone(void* pArg)
{
	CSardine_StateMachine* pInstance = new CSardine_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSardine_StateMachine::Free()
{
	__super::Free();
}
