#include "InputBuffer.h"

CInputBuffer::CInputBuffer()
{
}

HRESULT CInputBuffer::Initialize(_float fBufferDuration)
{
	// Buffer의 Duration을 설정
	m_fBufferDuration = fBufferDuration;

	return S_OK;
}

void CInputBuffer::Update(_float fTimeDelta)
{
	// BufferTime 계속해서 누적
	m_fBufferTime += fTimeDelta;

	// 큐 안을 순회하면서 현재의 BufferTime과 큐에 저장된 Input의 BufferTime 사이의 차이가 BufferDuration보다 크면 Input 제거
	while (!m_queInput.empty() && m_fBufferTime - m_queInput.front().second > m_fBufferDuration)
		m_queInput.pop();
}

void CInputBuffer::Add_Input(INPUTTYPE eInputType, _uint iKey)
{
	// Input 추가
	FInput tInput{ eInputType, iKey };

	m_queInput.push(make_pair(tInput, m_fBufferTime));
}

_bool CInputBuffer::Is_FirstInput(INPUTTYPE eInputType, _uint iKey)
{
	// 매개변수로 넘겨준 Key에 해당하는 Input이 큐의 마지막 요소인지를 확인
	// 마지막 요소가 맞다면 true 반환, 아니라면 false 반환
	if (m_queInput.empty()) return false;

	FInput tFirstInput = m_queInput.front().first;
	FInput tInput{ eInputType, iKey };

	_bool bFirstInput = tFirstInput == tInput;
	if (bFirstInput) m_queInput.pop();

	return bFirstInput;
}

void CInputBuffer::Clear()
{
	// 입력 버퍼 큐 초기화
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