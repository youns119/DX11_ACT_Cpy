#include "FSM.h"
#include "State.h"

CFSM::CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CFSM::CFSM(const CFSM& Prototype)
	: CComponent(Prototype)
{
}

HRESULT CFSM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFSM::Initialize(void* pArg)
{
	m_StateGroups.resize(8);

	FSM_DESC* pDesc = static_cast<FSM_DESC*>(pArg);
	if (pDesc == nullptr) {
		return E_FAIL;
	}
	else {
		m_eMachineMode = pDesc->eMode;
		m_pOwner = pDesc->pOwner;
	}	

	return S_OK;
}

void CFSM::Priority_Update(void* pObj, _float fTimedelta)
{
	Change_State(pObj);
}

void CFSM::Update(void* pObj, _float fTimedelta)
{
	if(nullptr != m_pCurrentState)
		Update_State(pObj, fTimedelta);
}

const _char* CFSM::Get_CurStateName()
{
	if (nullptr == m_pCurrentState)
		return "";

	return m_pCurrentState->Get_CurrentStateName();
}

void CFSM::Call_NextState(const _wstring _stateName)
{
	_bool isFound = false;
	for (int i = 0; i < m_StateGroups.size(); i++) {
		auto iter = m_StateGroups[i].find(_stateName);
		if (iter == m_StateGroups[i].end())
			continue;

		m_NextStates.push_back(iter->second);
	}
}

void CFSM::Change_State(void* pObj)
{
	if (m_NextStates.size() == 0)
		return;

	if (m_pCurrentState != nullptr)
		m_pCurrentState->Exit(pObj);

	m_pCurrentState = m_NextStates.back();
	m_NextStates.clear();

	m_pCurrentState->Enter(pObj);
}


void CFSM::Update_State(void* pObj, _float fTimeDelta)
{
	if (nullptr != m_pCurrentState)
		m_pCurrentState->Update(fTimeDelta, pObj);

	Cooldown_States(fTimeDelta);
}

void CFSM::Cooldown_States(_float fTimeDelta)
{
	for (int i = 0; i < m_StateGroups.size(); i++)
		for (auto& obj : m_StateGroups[i])
			obj.second->Cooldown(fTimeDelta);
}

void CFSM::Refresh_AllStates()
{
	for (int i = 0; i < m_StateGroups.size(); i++)
		for (auto& obj : m_StateGroups[i])
			obj.second->Refresh_CoolTime();
}

void CFSM::Clear_State()
{
	m_pCurrentState = nullptr;
	m_NextStates.clear();
}

void CFSM::Add_State(_uint iGoupIndex, const _wstring _szName, CState* _pState)
{
#ifndef _DEBUG
	_pState->AddRef();
#endif // !_DEBUG
	
	m_StateGroups[iGoupIndex].emplace(_szName, _pState);
}

void CFSM::Select_StatesFromGroup(_uint iGroupIndex)
{
	m_iCurGroup = iGroupIndex;

	_int iPriorLevel = 99;
	list<CState*> pPriorStates;
	vector<CState*> vecAvailableStates;

	for (auto& obj : m_StateGroups[iGroupIndex])
	{
		CState* temp = obj.second;
		if (temp->IsCooldown()) {
			if (pPriorStates.size() == 0)
				pPriorStates.push_back(temp);
			else if (pPriorStates.back()->Get_PriorLevel() > temp->Get_PriorLevel()) {
				pPriorStates.clear();
				pPriorStates.push_back(temp);
			}

			vecAvailableStates.push_back(temp);
		}
	}

	if (vecAvailableStates.size() == 0 || pPriorStates.size() == 0) {
		m_NextStates.push_back(m_pDefaultState);
		return;
	}

	if (m_eMachineMode == FSM_PRIOR) {
		if (pPriorStates.size() == 1) {
			m_NextStates.push_back(pPriorStates.back());
		}
		else {
			_uint iTemp = 0;
			_int iStateIndex = (_int)CUtils::Compute_Random(0, (_int)pPriorStates.size());
			for (auto& iter : pPriorStates)
			{
				if (iTemp == iStateIndex) {
					m_NextStates.push_back(iter);
					return;
				}
				iTemp++;
			}
		}
	}
	else if (m_eMachineMode == FSM_RAND) {
		_int iStateIndex = (_int)CUtils::Compute_Random(0, (_int)vecAvailableStates.size() - 1);
		m_NextStates.push_back(vecAvailableStates[iStateIndex]);
	}
}

void CFSM::Free()
{
	for (size_t i = 0; i < m_StateGroups.size(); i++) {
		for (auto obj : m_StateGroups[i]) {
			Safe_Release(obj.second);
		}

		m_StateGroups[i].clear();
	}

	if(nullptr != m_pDefaultState)
		Safe_Release(m_pDefaultState);

	m_StateGroups.clear();
	m_NextStates.clear();

	CComponent::Free();
}