#pragma once

#include "Client_Defines.h"
#include "Level.h"

#include "SpriteEffect.h"

/* 1. 다음 레벨이 필요로하는 자원을 준비한다. */
/* 2. 로딩화면을 구성(로딩화면에 필요한 객체를 만들고 갱신 렌더를 수행한다)한다. */

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

	_tchar m_szText[MAX_PATH] = L"로딩 중";

	_wstring m_strLoadingText[4] = { L"로딩 중", L"로딩 중.", L"로딩 중..", L"로딩 중..." };
	_int m_iCurIndex = {};
	_float m_fFrame = {};

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;

};

END
