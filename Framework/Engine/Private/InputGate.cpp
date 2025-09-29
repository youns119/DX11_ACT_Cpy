#include "InputGate.h"

CInputGate::CInputGate()
{
}

void CInputGate::Update(_float fTimeDelta)
{
	// �Է� ����Ʈ�� �ִ� ������ �ð��� ���� ����
	for (auto& Input : m_listInput)
		Input.second.fTime += fTimeDelta;
}

void CInputGate::Add_Input(INPUTTYPE eInputType, _uint iKey, _float fTimeThreshold)
{
	// �Ű������� �Ѱ��� Key�� �ش��ϴ� Input�� �߰�
	// ���� �̹� �����Ѵٸ� TimeThreshold�� Time�� �ʱ�ȭ���ش�
	_bool bExist{};
	FInput tInput{ eInputType, iKey };

	for (auto iter = m_listInput.begin(); iter != m_listInput.end();)
	{
		if (iter->first == tInput)
		{
			bExist = true;
			iter->second.fTimeThreshold = fTimeThreshold;
			iter->second.fTime = 0.f;
			break;
		}
		else iter++;
	}

	if(!bExist) m_listInput.push_back(make_pair(tInput, FThreshold{fTimeThreshold, 0.f}));
}

_bool CInputGate::Tap(INPUTTYPE eInputType, _uint iKey)
{
	// Tap���� �˻��Ѵ�
	// Time�� TimeThreshold���� ���� ���� Tap���� �����ؼ� Input�� �����ϰ� true ��ȯ
	_bool bTap{};

	for (auto iter = m_listInput.begin(); iter != m_listInput.end();)
	{
		FInput tInput{ eInputType, iKey };

		if (iter->first == tInput)
		{
			bTap = iter->second.fTimeThreshold > iter->second.fTime;

			if (!bTap)
				return bTap;
			else
			{
				m_listInput.erase(iter);
				return bTap;
			}
		}
		else iter++;
	}

	return bTap;
}

_bool CInputGate::Hold(INPUTTYPE eInputType, _uint iKey)
{
	// Hold���� �˻��Ѵ�
	// Time�� TimeThreshold���� ū ���� Hold�� �����ؼ� Input�� �����ϰ� true ��ȯ
	_bool bHold{};

	for (auto iter = m_listInput.begin(); iter != m_listInput.end();)
	{
		FInput tInput{ eInputType, iKey };

		if (iter->first == tInput)
		{
			bHold = iter->second.fTimeThreshold <= iter->second.fTime;

			if (!bHold)
				return bHold;
			else
			{
				m_listInput.erase(iter);
				return bHold;
			}
		}
		else iter++;
	}

	return bHold;
}

void CInputGate::Clear()
{
	m_listInput.clear();
}

CInputGate* CInputGate::Create()
{
	return new CInputGate;
}

void CInputGate::Free()
{
	super::Free();

	m_listInput.clear();
}