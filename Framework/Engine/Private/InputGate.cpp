#include "InputGate.h"

CInputGate::CInputGate()
{
}

void CInputGate::Update(_float fTimeDelta)
{
	// 입력 리스트에 있는 값들의 시간을 각각 누적
	for (auto& Input : m_listInput)
		Input.second.fTime += fTimeDelta;
}

void CInputGate::Add_Input(INPUTTYPE eInputType, _uint iKey, _float fTimeThreshold)
{
	// 매개변수로 넘겨준 Key에 해당하는 Input을 추가
	// 만약 이미 존재한다면 TimeThreshold와 Time만 초기화해준다
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
	// Tap인지 검사한다
	// Time이 TimeThreshold보다 작은 경우는 Tap으로 간주해서 Input을 삭제하고 true 반환
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
	// Hold인지 검사한다
	// Time이 TimeThreshold보다 큰 경우는 Hold로 간주해서 Input을 삭제하고 true 반환
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