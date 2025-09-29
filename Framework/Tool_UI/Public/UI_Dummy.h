#pragma once

#include "UITool_Defines.h"
#include "UIObject.h"

BEGIN(Engine)

class CTexture;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(UITool)

class CUI_Dummy final : public CUIObject
{
public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_Dummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Dummy(const CUI_Dummy& Prototype);
	virtual ~CUI_Dummy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Clone_Texture(_uint iLevelIndex, const _wstring& strTextureTag);
	void Clone_MaskTexture(_uint iLevelIndex, const _wstring& strMaskTextureTag);

public:
	const _float2& Get_Scale() const { return m_vScale; }
	void				Set_Scale(_float2 vScale);

	const _float3& Get_Locate() const { return m_vLocate; }
	void				Set_Locate(_float3 vLocate);



	const _float3& Get_Axis() const { return m_vAxis; }
	void				Set_Axis(_float3 vAxis) { m_vAxis = vAxis; }

	const _float		Get_Degrees() const { return m_fDegrees; }
	void				Set_Degrees(_float fDegrees) { m_fDegrees = fDegrees; }



	const _float2& Get_SpriteSize() const { return m_vSpriteSize; }
	void				Set_SpriteSize(_float2 vSpriteSize) { m_vSpriteSize = vSpriteSize; }

	const _float2& Get_SpriteIndex() const { return m_vSpriteIndex; }
	void				Set_SpriteIndex(_float2 vSpriteIndex) { m_vSpriteIndex = vSpriteIndex; }

	const _float& Get_Threshold() const { return m_fThreshold; }
	void				Set_Threshold(_float fThreshold) { m_fThreshold = fThreshold; }

	const _float& Get_Interval() const { return m_fInterval; }
	void				Set_Interval(_float fInterval) { m_fInterval = fInterval; }


	const _float2& Get_StretchRange() const { return m_vStretchRange; }
	void				Set_StretchRange(_float2 vStretchRange) { m_vStretchRange = vStretchRange; }

	const _float& Get_StretchFactor() const { return m_fStretchFactor; }
	void				Set_StretchFactor(_float fStretchFactor) { m_fStretchFactor = fStretchFactor; }

	const _tchar* Get_Text() const { return m_szText; }
	void				Set_Text(const _tchar* szText) { lstrcpy(m_szText, szText); }

	const _float2		Get_TextOffset() const { return m_vTextOffset; }
	void				Set_TextOffset(_float2 vTextOffset) { m_vTextOffset = vTextOffset; }

	const _float& Get_TextScale() const { return m_fTextScale; }
	void				Set_TextScale(_float fTextScale) { m_fTextScale = fTextScale; }

	const _float4& Get_TextColor() const { return m_vTextColor; }
	void				Set_TextColor(_float4 vTextColor) { m_vTextColor = vTextColor; }

	const _float4& Get_HoverTextColor() const { return m_vHoverTextColor; }
	void				Set_HoverTextColor(_float4 vHoverTextColor) { m_vHoverTextColor = vHoverTextColor; }

	void Set_FontSortType(SORTING eSorting) { m_eSorting = eSorting; }

	const _wstring& Get_FontTag() const { return m_strFontTag; }
	void				Set_FontTag(_wstring strFontTag) { m_strFontTag = strFontTag; }

	const _wstring& Get_TextureTag() const { return m_strTextureTag; }
	const _wstring& Get_MaskTextureTag() const { return m_strMaskTextureTag; }


	_uint				Get_ShaderPass() const { return m_iShaderPass; }
	void				Set_ShaderPass(_uint iShaderPass) { m_iShaderPass = iShaderPass; }

	const _float4& Get_SrcColor() const { return m_vSrcColor; }
	void				Set_SrcColor(_float4 vSrcColor) { m_vSrcColor = vSrcColor; }

	const _float4& Get_DstColor() const { return m_vDstColor; }
	void				Set_DstColor(_float4 vDstColor) { m_vDstColor = vDstColor; }

	void				Set_RightAngle(_bool bRightAngle) { m_bRightAngle = bRightAngle; }
	void				Set_StretchY(_bool bStretchY) { m_bStretchY = bStretchY; }

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

	CTexture*			m_pMaskTextureCom = { nullptr };

	_float2				m_vScale = { g_iWindowSizeX, g_iWindowSizeY };
	_float3				m_vLocate = { g_iWindowSizeX * 0.5f, g_iWindowSizeY * 0.5f, 0.f};

	_float3				m_vAxis = {0.f, 0.f, 1.f};
	_float				m_fDegrees = {};


	_float2				m_vSpriteSize = {};
	_float2				m_vSpriteIndex = {};

	_float				m_fThreshold = {};
	_float				m_fInterval = {};


	_float2				m_vStretchRange = { };

	// 저장할 필요 없을듯?
	_float				m_fStretchFactor = { 1.0f };


	_tchar				m_szText[MAX_PATH] = {};
	_float2				m_vTextOffset = {};
	_float				m_fTextScale = {};
	_float4				m_vTextColor = { 0.f, 0.f, 0.f, 1.f };
	_float4				m_vHoverTextColor = { 1.f, 1.f, 1.f, 1.f };


	_wstring			m_strTextureTag = L"";
	_wstring			m_strFontTag = L"";
	_wstring			m_strMaskTextureTag = L"";

	_bool				m_bRightAngle = { false };
	_bool				m_bStretchY = { false };

	_uint				m_iShaderPass = {0};
	_float4				m_vSrcColor = { 0.f, 0.f, 0.f, 1.f };
	_float4				m_vDstColor = { 0.f, 0.f, 0.f, 1.f };

	SORTING				m_eSorting = {SORTING::NONE};

	_bool				m_bHovered = { false };

	_float				m_fFrame = { 0.f };
private:
	HRESULT Ready_Components();

public:
	static CUI_Dummy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
