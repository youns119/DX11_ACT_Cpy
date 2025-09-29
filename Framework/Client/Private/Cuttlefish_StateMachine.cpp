#include "pch.h"
#include "Cuttlefish_StateMachine.h"
#include "GameInstance.h"

//States
#include "Cuttlefish_State_Aggroed.h"
#include "Cuttlefish_State_Capsize.h"
#include "Cuttlefish_State_Death.h"
#include "Cuttlefish_State_Execution.h"
#include "Cuttlefish_State_Hooked.h"
#include "Cuttlefish_State_Punch.h"
#include "Cuttlefish_State_Stagger.h"
#include "Cuttlefish_State_TeleportAttack.h"
#include "Cuttlefish_State_ThrowProjectile.h"
#include "Cuttlefish_State_Wait.h"
#include "Cuttlefish_State_Move.h"
#include "Cuttlefish_State_UnaggroedWait.h"

CCuttlefish_StateMachine::CCuttlefish_StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CCuttlefish_StateMachine::CCuttlefish_StateMachine(const CCuttlefish_StateMachine& Prototype)
	: super(Prototype)
{
}

HRESULT CCuttlefish_StateMachine::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}
	
HRESULT CCuttlefish_StateMachine::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;
	
	m_pDefaultState = CCuttlefish_State_Wait::Create(m_pOwner);
	CState* pMove = CCuttlefish_State_Move::Create(m_pOwner);
	CState* pThrow_Projectile = CCuttlefish_State_ThrowProjectile::Create(m_pOwner);
	CState* pExecution = CCuttlefish_State_Execution::Create(m_pOwner);

	Add_State((_uint)STATE_UNAGGRO, L"Unaggroed", CCuttlefish_State_UnaggroedWait::Create(m_pOwner));

	Add_State((_uint)STATE_CLOSE, L"Punch", CCuttlefish_State_Punch::Create(m_pOwner));
	Add_State((_uint)STATE_CLOSE, L"Execution", pExecution);
	Add_State((_uint)STATE_CLOSE, L"Move", pMove);

	Add_State((_uint)STATE_RANGED, L"Teleport_Attack", CCuttlefish_State_TeleportAttack::Create(m_pOwner));
	Add_State((_uint)STATE_RANGED, L"Throw_Projectile", pThrow_Projectile);
	Add_State((_uint)STATE_RANGED, L"Move", pMove);
	Add_State((_uint)STATE_CLOSE, L"Execution", pExecution);
	Add_State((_uint)STATE_RANGED, L"Wait", CCuttlefish_State_Wait::Create(m_pOwner));

	Add_State((_uint)STATE_FAR, L"Throw_Projectile", pThrow_Projectile);
	
	Add_State((_uint)STATE_OTHER, L"Death", CCuttlefish_State_Death::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Capsized", CCuttlefish_State_Capsize::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Stagger", CCuttlefish_State_Stagger::Create(m_pOwner));
	Add_State((_uint)STATE_OTHER, L"Aggroed", CCuttlefish_State_Aggroed::Create(m_pOwner));	
	Add_State((_uint)STATE_OTHER, L"Hooked", CCuttlefish_State_Hooked::Create(m_pOwner));
	
	return S_OK;
}

void CCuttlefish_StateMachine::Priority_Update(void* pObj, _float fTimeDelta)
{
	super::Priority_Update(pObj, fTimeDelta);
}

void CCuttlefish_StateMachine::Update(void* pObj, _float fTimeDelta)
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

CCuttlefish_StateMachine* CCuttlefish_StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCuttlefish_StateMachine* pInstance = new CCuttlefish_StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCuttlefish_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCuttlefish_StateMachine::Clone(void* pArg)
{
	CCuttlefish_StateMachine* pInstance = new CCuttlefish_StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_StateMachine::Free()
{
	__super::Free();
}
