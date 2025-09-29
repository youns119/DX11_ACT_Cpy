#include "AnimState.h"

#include "StateMachine.h"

CAnimState::CAnimState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CAnimState::Initialize(CStateMachine* pOwner)
{
	// ���¸� ������ ��ü -> ���¸ӽ�
	m_pOwner = pOwner;

	return S_OK;
}

CGameObject* CAnimState::Get_Character()
{
	// ���¸ӽ��� �����ϴ� ��ü ��ȯ -> ĳ���� ��ȯ
	return m_pOwner->Get_Owner();
}

void CAnimState::Free()
{
	super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}