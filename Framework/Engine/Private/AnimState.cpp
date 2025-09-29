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
	// 상태를 가지는 객체 -> 상태머신
	m_pOwner = pOwner;

	return S_OK;
}

CGameObject* CAnimState::Get_Character()
{
	// 상태머신을 소유하는 객체 반환 -> 캐릭터 반환
	return m_pOwner->Get_Owner();
}

void CAnimState::Free()
{
	super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}