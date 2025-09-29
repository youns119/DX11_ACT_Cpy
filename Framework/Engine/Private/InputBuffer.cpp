#include "InputBuffer.h"

CInputBuffer::CInputBuffer()
{
}

HRESULT CInputBuffer::Initialize(_float fBufferDuration)
{
	// Buffer�� Duration�� ����
	m_fBufferDuration = fBufferDuration;

	return S_OK;
}

void CInputBuffer::Update(_float fTimeDelta)
{
	// BufferTime ����ؼ� ����
	m_fBufferTime += fTimeDelta;

	// ť ���� ��ȸ�ϸ鼭 ������ BufferTime�� ť�� ����� Input�� BufferTime ������ ���̰� BufferDuration���� ũ�� Input ����
	while (!m_queInput.empty() && m_fBufferTime - m_queInput.front().second > m_fBufferDuration)
		m_queInput.pop();
}

void CInputBuffer::Add_Input(INPUTTYPE eInputType, _uint iKey)
{
	// Input �߰�
	FInput tInput{ eInputType, iKey };

	m_queInput.push(make_pair(tInput, m_fBufferTime));
}

_bool CInputBuffer::Is_FirstInput(INPUTTYPE eInputType, _uint iKey)
{
	// �Ű������� �Ѱ��� Key�� �ش��ϴ� Input�� ť�� ������ ��������� Ȯ��
	// ������ ��Ұ� �´ٸ� true ��ȯ, �ƴ϶�� false ��ȯ
	if (m_queInput.empty()) return false;

	FInput tFirstInput = m_queInput.front().first;
	FInput tInput{ eInputType, iKey };

	_bool bFirstInput = tFirstInput == tInput;
	if (bFirstInput) m_queInput.pop();

	return bFirstInput;
}

void CInputBuffer::Clear()
{
	// �Է� ���� ť �ʱ�ȭ
	while (!m_queInput.empty())
		m_queInput.pop();

	m_fBufferTime = 0.f;
}

CInputBuffer* CInputBuffer::Create(_float fBufferDuration)
{
	CInputBuffer* pInstance = new CInputBuffer;

	if (FAILED(pInstance->Initialize(fBufferDuration)))
	{
		MSG_BOX("Failed To Create : CInputBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInputBuffer::Free()
{
	super::Free();

	Clear();
}