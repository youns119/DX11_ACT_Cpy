#pragma once

#include "Tool_Define.h"
#include "Level.h"
#include "MapLoader.h"
/* 1. ���� ������ �ʿ���ϴ� �ڿ��� �غ��Ѵ�. */
/* 2. �ε�ȭ���� ����(�ε�ȭ�鿡 �ʿ��� ��ü�� ����� ���� ������ �����Ѵ�)�Ѵ�. */

BEGIN(MapTool)

class CLoader;

class CMapLevel_Loading final : public CLevel
{
private:
	CMapLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMapLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVELID m_eNextLevelID{ LEVELID::LEVEL_END };
	CMapLoader* m_pLoader{ nullptr };

	_float m_fCurrentProgress{ 0.f };

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

public:
	static CMapLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;

};

END
