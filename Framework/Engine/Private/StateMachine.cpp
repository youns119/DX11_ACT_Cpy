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

	// ���¸� ������ ��ü -> ĳ����
	m_pOwner = pDesc->pOwner;

	if (FAILED(Ready_AnimState()))
		return E_FAIL;

	// ���� ���� ���°� ��ȿ�ϸ� Enter
	m_pCurrAnimState = Find_AnimState(pDesc->strState);
	if (m_pCurrAnimState)
		m_pCurrAnimState->Enter();

	return S_OK;
}

void CStateMachine::Update(_float fTimeDelta)
{
	// ���� ������Ʈ ������Ʈ
	if (m_pCurrAnimState)
		m_pCurrAnimState->Update(fTimeDelta);
}

HRESULT CStateMachine::ChangeState(const _wstring& strState)
{
	// ���� ���°� ��ȿ�ϴ��� Ž���ϰ�
	// ��ȿ�ϸ� ���� ���� Exit, ���ο� ���� Enter
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
	// ���ο� ���¸� �߰��Ѵ�
	// �̹� �ְų� nullptr��� E_FAIL ��ȯ
	if (pAnimState == nullptr ||
		Find_AnimState(strAnimState) != nullptr)
		return E_FAIL;

	m_mapAnimState.emplace(strAnimState, pAnimState);

	return S_OK;
}

CAnimState* CStateMachine::Find_AnimState(const _wstring& strAnimState)
{
	// ���°� �ִ��� Ȯ���Ѵ�
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