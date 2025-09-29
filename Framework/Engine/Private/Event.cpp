#include "Event.h"

// 커스텀하기 자유롭게 하기 위해서 되도록이면 거의 빈 클래스로 만들었다
// GameObject를 기본 매개변수로 한 이유는 GameObject는 거의 모든 상황에 필요할 것 같아서 넣어놨다
CEvent::CEvent()
{
}

HRESULT CEvent::Init(CGameObject* pGameObject)
{
	m_pGameObject = pGameObject;

	return S_OK;
}

void CEvent::Free()
{
	super::Free();
}