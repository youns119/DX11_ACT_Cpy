#include "State.h"
#include "GameInstance.h"

CState::CState()
{
}

HRESULT CState::Initialize(void* pObj)
{
	m_fCurCoolTime = m_fMaxCoolTime;

	STATE_DESC* pDesc = static_cast<STATE_DESC*>(pObj);
	if (pDesc == nullptr)
		return S_OK;
	else
		m_pOwner = pDesc->pOwner;

	return S_OK;
}

void CState::Enter(void* pObj)
{
	m_fCurCoolTime = 0;
}

void CState::Update(_float fTimeDelta, void* pObj)
{
}

void CState::Exit(void* pObj)
{
}

void CState::Refresh_CoolTime()
{
	m_fCurCoolTime = m_fMaxCoolTime;
}

void CState::Cooldown(_float fTimeDelta)
{
	if (m_fCurCoolTime == m_fMaxCoolTime)
		return;

	m_fCurCoolTime += fTimeDelta;

	if (m_fCurCoolTime >= m_fMaxCoolTime)
		m_fCurCoolTime = m_fMaxCoolTime;
}

void CState::Free()
{
	__super::Free();
}