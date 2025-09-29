#include "Level_Manager.h"

#include "Level.h"
#include "GameInstance.h"


CLevel_Manager::CLevel_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Open_Level(_uint iNewLevelIndex, CLevel* pNewLevel)
{
	m_NextLevels.push_back({ iNewLevelIndex, pNewLevel });

	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if (!m_NextLevels.empty())
	{
		_uint iNextLevelIndex = m_NextLevels.back().first;
		CLevel* pNextLevel = m_NextLevels.back().second;
		m_NextLevels.pop_back();

		if (nullptr != m_pCurrentLevel)
		{
			if (m_iLevelIndex != iNextLevelIndex)
				m_pGameInstance->Clear_Prototype(m_iLevelIndex); // 멀티 스레드라서 프로토 타입을 지우는 시점과 프로토타입을 만드는 시점이 동기화가 안되었다. 그래서 clear_Prototype 함수가 호출되기 전에 다른 스레드에서 커포넌트 만들어버림
			m_pGameInstance->Clear_Object(m_iLevelIndex);
			//m_pGameInstance->Stop_Sound_All();
			m_pGameInstance->Clear_SoundManager();
			m_pGameInstance->Clear_CollisionManager();
			m_pGameInstance->Clear_Lights();
			m_pGameInstance->Clear_PhysicsManager();
			m_pGameInstance->Clear_Effects();
			m_pGameInstance->Clear_CameraManager();
			Safe_Release(m_pCurrentLevel);
		}


		m_iLevelIndex = iNextLevelIndex;
		m_pCurrentLevel = pNextLevel;

		if (FAILED(m_pCurrentLevel->Initialize()))
		{
			MSG_BOX("failed to Initailize level");
		}

		for (auto& Pair : m_NextLevels)
			Safe_Release(Pair.second);
		m_NextLevels.clear();
	}

	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	return m_pCurrentLevel->Render();
}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	CBase::Free();

	Safe_Release(m_pCurrentLevel);
	Safe_Release(m_pGameInstance);
}
