#pragma once

#include "UITool_Defines.h"
#include "Level.h"
#include "UIObject.h"

BEGIN(UITool)

class CUI_Dummy;

class CLevel_UITool final : public CLevel
{
private:
	CLevel_UITool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_UITool() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Tags();

	void UI_Menu();

	void Initialize_UI();
	void Create_UI();
	void Delete_UI();

	void Save_UI();
	void Load_UI();

private:
	CUI_Dummy* m_pUI = { nullptr };

	vector<string> m_Shaderlabels = { "DEFAULT", "BLEND", "COLOR", "COLOR_BLEND", "RING", "FILTER_BLEND", "MENU_BUTTON", "OPTION_BUTTON", "CHATBUBBLE", "SPRITE", "BUTTON", "INVENTORY", "LOCKON", "STRETCH", "HURT", "STRETCH_BAR", "STRETCH_BAR_EFFECT", "NOISE", "NOISE_EDGE", "COLOR_NONCULL", "BORDER", "STRETCH_DEFAULT", "SHOP_BORDER", "OUTLINE", "SHELL_BORDER", "SKILL", "GAUGE"};

	vector<_wstring> m_TextureTag;
	vector<_wstring> m_SpriteTextureTag;
	vector<_wstring> m_MaskTextureTag;
	vector<_wstring> m_FontTag;

	_float2 m_vResolution = {};

	_float2		m_vScale = { g_iWindowSizeX, g_iWindowSizeY};
	_float3		m_vLocate = {};

	_float3		m_vAxis = {};
	_float		m_fDegrees = {};

	_float2 m_vSpriteSize = {};
	_float2 m_vSpriteIndex = {};

	_float	m_fInterval = {};
	_float	m_fThreshold = {};

	_float2	m_vStretchRange = { };
	_float	m_fStretchFactor = { 1.0f };


	_float2 m_vTextOffset = {};
	_float	m_fTextScale = {};
	_float4	m_vTextColor = { 1.f, 1.f, 1.f, 1.f };
	_float4	m_vHoverTextColor = { 1.f, 1.f, 1.f, 1.f };

	_float4	m_vSrcColor = { 1.f, 1.f, 1.f, 1.f };
	_float4	m_vDstColor = { 0.f, 0.f, 0.f, 1.f };

	_char m_szInputText[MAX_PATH] = "";

	_int m_iFontSelect = {};
	_int m_iShaderPass = {};

	_int m_iCount = {};
	_int m_iCurIndex = {};

	_bool m_bRightAngle = {};
	_bool m_bStretchY = {};

	_int m_iSortingNum = {};

public:
	static CLevel_UITool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END