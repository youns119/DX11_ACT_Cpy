#include "pch.h"
#include "BobbitWorm_StateMachine.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"

//States
#include "BobbitWorm_State_Wait.h"
#include "BobbitWorm_State_Death.h"
#include "BobbitWorm_State_Ambush.h"

CBobbitWorm_StateMachine::CBobbitWorm_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBobbitWorm_StateMachine::CBobbitWorm_StateMachine(const CBobbitWorm_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CBobbitWorm_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CBobbitWorm_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;
	
	m_pDefaultState = CBobbitWorm_State_Wait::Create(m_pOwner);

	Add_State((_uint)STATE_OTHER, L"Ambush", CBobbitWorm_State_Ambush::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Wait", CBobbitWorm_State_Wait::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Death", CBobbitWorm_State_Death::Create(m_pOwner));

	return S_OK;
}

void CBobbitWorm_StateMachine::Priority_Update(void* pObj, _float fTimeDelta)
{
	super::Priority_Update(pObj, fTimeDelta);
}

void CBobbitWorm_StateMachine::Update(void* pObj, _float fTimeDelta)
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

CBobbitWorm_StateMachine* CBobbitWorm_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBobbitWorm_StateMachine* pInstance = new CBobbitWorm_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBobbitWorm_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBobbitWorm_StateMachine::Clone(void* pArg)
{
	CBobbitWorm_StateMachine* pInstance = new CBobbitWorm_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBobbitWorm_StateMachine::Free()
{
	__super::Free();
}
