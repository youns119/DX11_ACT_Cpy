#include "StateMachine.h"

#include "AnimState.h"

CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CStateMachine::CStateMachine(const CStateMachine& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CStateMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	// 상태를 가지는 객체 -> 캐릭터
	m_pOwner = pDesc->pOwner;

	if (FAILED(Ready_AnimState()))
		return E_FAIL;

	// 현재 들어온 상태가 유효하면 Enter
	m_pCurrAnimState = Find_AnimState(pDesc->strState);
	if (m_pCurrAnimState)
		m_pCurrAnimState->Enter();

	return S_OK;
}

void CStateMachine::Update(_float fTimeDelta)
{
	// 현재 스테이트 업데이트
	if (m_pCurrAnimState)
		m_pCurrAnimState->Update(fTimeDelta);
}

HRESULT CStateMachine::ChangeState(const _wstring& strState)
{
	// 들어온 상태가 유효하는지 탐색하고
	// 유효하면 현재 상태 Exit, 새로운 상태 Enter
	CAnimState* pAnimState = Find_AnimState(strState);
	if (pAnimState == nullptr)
		return E_FAIL;

	if (m_pCurrAnimState)
		m_pCurrAnimState->Exit();

	m_pCurrAnimState = pAnimState;
	m_pCurrAnimState->Enter();

	return S_OK;
}

HRESULT CStateMachine::Add_AnimState(const _wstring& strAnimState, CAnimState* pAnimState)
{
	// 새로운 상태를 추가한다
	// 이미 있거나 nullptr라면 E_FAIL 반환
	if (pAnimState == nullptr ||
		Find_AnimState(strAnimState) != nullptr)
		return E_FAIL;

	m_mapAnimState.emplace(strAnimState, pAnimState);

	return S_OK;
}

CAnimState* CStateMachine::Find_AnimState(const _wstring& strAnimState)
{
	// 상태가 있는지 확인한다
	auto iter = m_mapAnimState.find(strAnimState);
	if (iter == m_mapAnimState.end())
		return nullptr;

	return iter->second;
}

void CStateMachine::Free()
{
	super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& Pair : m_mapAnimState)
		Safe_Release(Pair.second);

	m_mapAnimState.clear();
}