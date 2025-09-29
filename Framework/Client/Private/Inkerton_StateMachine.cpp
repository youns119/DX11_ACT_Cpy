#include "pch.h"
#include "Inkerton_StateMachine.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"

//States
#include "Inkerton_State_Wait.h"
#include "Inkerton_State_Death.h"
#include "Inkerton_State_Hooked.h"
#include "Inkerton_State_Aggroed.h"
#include "Inkerton_State_Stagger.h"
#include "Inkerton_State_GunShot.h"
#include "Inkerton_State_InkSpurt.h"
#include "Inkerton_State_Capsize.h"
#include "Inkerton_State_Retreat.h"
#include "Inkerton_State_SwimBack.h"
#include "Inkerton_State_SwimFront.h"
#include "Inkerton_State_DodgeBack.h"
#include "Inkerton_State_Execution.h"
#include "Inkerton_State_DodgeSide.h"
#include "Inkerton_State_LightAttack.h"
#include "Inkerton_State_ComboAttack.h"
#include "Inkerton_State_DodgeLeftAttack.h"
#include "Inkerton_State_InkAmbushAttack.h"
#include "Inkerton_State_DodgeRightAttack.h"
#include "Inkerton_State_HeavyChargeAttack.h"
#include "Inkerton_State_LightChargeAttack.h"

CInkerton_StateMachine::CInkerton_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CInkerton_StateMachine::CInkerton_StateMachine(const CInkerton_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CInkerton_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CInkerton_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;

	IMKERTON_FSM_DESC* pDesc = static_cast<IMKERTON_FSM_DESC*>(pArg);

	if (FAILED(Ready_States()))
		return E_FAIL;

	return S_OK;
}

void CInkerton_StateMachine::Priority_Update(void* pObj, _float ftimedelta)
{
	super::Priority_Update(pObj, ftimedelta);
}

void CInkerton_StateMachine::Update(void* pObj, _float ftimedelta)
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

HRESULT CInkerton_StateMachine::Ready_States()
{
	m_pDefaultState = CInkerton_State_Wait::Create(m_pOwner);
	CState* pDodgeLeft = CInkerton_State_DodgeSide::Create(m_pOwner);
	CState* pSwimFront = CInkerton_State_SwimFront::Create(m_pOwner);
	CState* pHeavyChargeAttack = CInkerton_State_HeavyChargeAttack::Create(m_pOwner); 
	CState* pLightChargeAttack = CInkerton_State_LightChargeAttack::Create(m_pOwner);	
	CState* pInkAmbushAttack = CInkerton_State_InkAmbushAttack::Create(m_pOwner);


	/* Close */
	{
		Add_State((_uint)STATE_CLOSE, L"Dodge_Left", pDodgeLeft);
		Add_State((_uint)STATE_CLOSE, L"Dodge_Back", CInkerton_State_DodgeBack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Swim_Back", CInkerton_State_SwimBack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Dodge_LeftAttack", CInkerton_State_DodgeLeftAttack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Combo_Attack", CInkerton_State_ComboAttack::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Ink_Spurt", CInkerton_State_InkSpurt::Create(m_pOwner));
		Add_State((_uint)STATE_CLOSE, L"Ink_AmbushAttack", pInkAmbushAttack);
		Add_State((_uint)STATE_CLOSE, L"Execution", CInkerton_State_Execution::Create(m_pOwner));
	}

	/* Ranged */
	{
		Add_State((_uint)STATE_RANGED, L"Dodge_Left", pDodgeLeft);
		Add_State((_uint)STATE_RANGED, L"Light_ChargeAttack", pLightChargeAttack);
		Add_State((_uint)STATE_RANGED, L"Swim_Front", pSwimFront);
		Add_State((_uint)STATE_RANGED, L"Wait", m_pDefaultState);
		Add_State((_uint)STATE_RANGED, L"Gun_Shot", CInkerton_State_GunShot::Create(m_pOwner));
		Add_State((_uint)STATE_RANGED, L"Dodge_RightAttack", CInkerton_State_DodgeRightAttack::Create(m_pOwner));
		Add_State((_uint)STATE_RANGED, L"Heavy_ChargeAttack", pHeavyChargeAttack);
		Add_State((_uint)STATE_RANGED, L"Ink_AmbushAttack", pInkAmbushAttack);	
	}

	/* Far */
	{
		Add_State((_uint)STATE_FAR, L"Gun_Shot", CInkerton_State_GunShot::Create(m_pOwner));
		Add_State((_uint)STATE_FAR, L"Swim_Front", pSwimFront);
		Add_State((_uint)STATE_FAR, L"Heavy_ChargeAttack", pHeavyChargeAttack);
		Add_State((_uint)STATE_FAR, L"Ink_AmbushAttack", pInkAmbushAttack);
	}

	/* Other */
	{
		Add_State((_uint)STATE_OTHER, L"Death", CInkerton_State_Death::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Retreat", CInkerton_State_Retreat::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Capsized", CInkerton_State_Capsize::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Stagger", CInkerton_State_Stagger::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Aggroed", CInkerton_State_Aggroed::Create(m_pOwner));
		Add_State((_uint)STATE_OTHER, L"Hooked", CInkerton_State_Hooked::Create(m_pOwner));
	}

	return S_OK;
}

CInkerton_StateMachine* CInkerton_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInkerton_StateMachine* pInstance = new CInkerton_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CInkerton_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CInkerton_StateMachine::Clone(void* pArg)
{
	CInkerton_StateMachine* pInstance = new CInkerton_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_StateMachine::Free()
{
	__super::Free();
}
