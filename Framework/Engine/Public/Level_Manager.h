#pragma once

#include "Base.h"

/* 1. ���� �Ҵ�� ������ �ּҸ� �����Ѵ�. */
/* 2. �����ϰ� �ִ� ������ �ݺ����� ���Ű� ��ο츦 �����Ѵ�. */
/* 3. ���ο� ������ �Ҵ� ��, ���� ������ �ı��ϰ� ���� ������ �ڿ��鵵 �ı��Ѵ�. */

BEGIN(Engine)

class CLevel;
class CGameInstance;

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iNewLevelIndex, CLevel* pNewLevel);
	_uint Get_CurrentLevelIndex() const { return m_iLevelIndex; }

	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	_uint m_iLevelIndex { 0 };
	CLevel* m_pCurrentLevel { nullptr };
	CGameInstance* m_pGameInstance { nullptr };

	vector<pair<_uint, CLevel*>> m_NextLevels;

public:
	static CLevel_Manager* Create();
	virtual void Free() override;

};

END
