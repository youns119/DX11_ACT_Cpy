#include "Event.h"

// Ŀ�����ϱ� �����Ӱ� �ϱ� ���ؼ� �ǵ����̸� ���� �� Ŭ������ �������
// GameObject�� �⺻ �Ű������� �� ������ GameObject�� ���� ��� ��Ȳ�� �ʿ��� �� ���Ƽ� �־����
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