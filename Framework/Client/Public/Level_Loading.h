#pragma once

#include "Client_Defines.h"
#include "Level.h"

#include "SpriteEffect.h"

/* 1. ���� ������ �ʿ���ϴ� �ڿ��� �غ��Ѵ�. */
/* 2. �ε�ȭ���� ����(�ε�ȭ�鿡 �ʿ��� ��ü�� ����� ���� ������ �����Ѵ�)�Ѵ�. */

BEGIN(Client)

class CLoader;
class CUI_Manager;

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVELID m_eNextLevelID{ LEVELID::LEVEL_END };
	CLoader* m_pLoader{ nullptr };

	_float m_fCurrentProgress{ 0.f };

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Effect();

private:
	void ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc);

private:
	CUI_Manager* m_pUIManager = { nullptr };

	_tchar m_szText[MAX_PATH] = L"�ε� ��";

	_wstring m_strLoadingText[4] = { L"�ε� ��", L"�ε� ��.", L"�ε� ��..", L"�ε� ��..." };
	_int m_iCurIndex = {};
	_float m_fFrame = {};

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;

};

END
