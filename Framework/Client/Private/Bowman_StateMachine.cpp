#include "pch.h"
#include "Bowman_StateMachine.h"
#include "GameInstance.h"
#include "Enemy_Bowman.h"

//States
#include "Bowman_State_Wait.h"
#include "Bowman_State_Hooked.h"
#include "Bowman_State_Death.h"
#include "Bowman_State_Stagger.h"
#include "Bowman_State_Capsize.h"
#include "Bowman_State_Backflip.h"
#include "Bowman_State_ClawAttack.h"
#include "Bowman_State_SnapAttack.h"
#include "Bowman_State_BashAttack.h"
#include "Bowman_State_RangeAttack.h"

CBowman_StateMachine::CBowman_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBowman_StateMachine::CBowman_StateMachine(const CBowman_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CBowman_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CBowman_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;

	m_pDefaultState = CBowman_State_Wait::Create(m_pOwner);

	Add_State((_uint)STATE_UNAGGRO, L"Wait", CBowman_State_Wait::Create(m_pOwner));

	Add_State((_uint)STATE_CLOSE, L"Backflip", CBowman_State_Backflip::Create(m_pOwner));
	Add_State((_uint)STATE_CLOSE, L"Bash_Attack", CBowman_State_BashAttack::Create(m_pOwner));
	Add_State((_uint)STATE_CLOSE, L"Claw_Attack", CBowman_State_ClawAttack::Create(m_pOwner));	
	Add_State((_uint)STATE_CLOSE, L"Snap_Attack", CBowman_State_SnapAttack::Create(m_pOwner));

	Add_State((_uint)STATE_RANGED, L"Range_Attack", CBowman_State_RangeAttack::Create(m_pOwner));
	Add_State((_uint)STATE_RANGED, L"Wait", CBowman_State_Wait::Create(m_pOwner));

	Add_State((_uint)STATE_OUTOFRANGE, L"Wait", CBowman_State_Wait::Create(m_pOwner));	

	Add_State((_uint)STATE_OTHER, L"Death", CBowman_State_Death::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Capsized", CBowman_State_Capsize::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Stagger", CBowman_State_Stagger::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Hooked", CBowman_State_Hooked::Create(m_pOwner));	

	return S_OK;
}

void CBowman_StateMachine::Priority_Update(void* pObj, _float ftimedelta)
{
	super::Priority_Update(pObj, ftimedelta);
}

void CBowman_StateMachine::Update(void* pObj, _float ftimedelta)
{
	super::Update(pObj, ftimedelta);

#ifdef _DEBUG
	//ImGui::Begin("State Debugger");

	//ImGui::NewLine();
	//
	//ImGui::Text("State Group: %s", m_Mode[m_eMachineMode]);
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

CBowman_StateMachine* CBowman_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBowman_StateMachine* pInstance = new CBowman_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBowman_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBowman_StateMachine::Clone(void* pArg)
{
	CBowman_StateMachine* pInstance = new CBowman_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowman_StateMachine::Free()
{
	__super::Free();
}

