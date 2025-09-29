#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)

class CRenderer;

class CTexture;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CUI : public CUIObject
{
public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		CUIObject* pOwner = {nullptr};
		const _tchar* strFileName = L"";

		_tchar szText[MAX_PATH] = {};
		LEVELID eLevelID = { LEVEL_STATIC };
		_bool	bInWorld = { false };
		_float fWorldOffsetY = { 0.f };
		_bool	isActive = { true };
		_bool	bShake = { false };
		_bool	bRightAngle = { false };
		_bool	bStretchY = { false };
		SORTING eSorting = SORTING::NONE;
	};

	struct UI_PARAMS
	{
		_float4 vSrcColor = { 0.f, 0.f, 0.f, 1.f };
		_float4 vDstColor = { 0.f, 0.f, 0.f, 1.f };

		_float2 vSpriteSize = {};
		_float2 vSpriteIndex = {};

		_float2 vStretchRange = {};
		_float  fThreshold = {};
		_float	fPadding = {};
	};


protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& Prototype);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_Locate(_float3 vLocate);
	_float3 Get_Locate() { return m_vLocate; }

	void Set_Scale(_float2 vScale);
	_float2 Get_Scale() { return m_vScale; }

	void Set_SpriteXY(_int iSpriteX, _int iSpriteY) { m_tParams.vSpriteIndex.x = iSpriteX; m_tParams.vSpriteIndex.y = iSpriteY; }
	void Set_CoverTexture(_wstring strTextureTag, _uint iTextureNum);
	void Delete_CoverTexture();

	void Change_Texture(_wstring strTextureTag, _uint iTextureNum);
	void Change_TextureNum(_uint iTextureNum) {m_iTextureNum = iTextureNum;}
	_uint Get_TextureNum() { return m_iTextureNum;}
	_wstring Get_TextureTag() { return m_strTextureTag;}

	void Change_SrcColor(_float4 vColor) { m_tParams.vSrcColor = vColor; }
	void Change_DstColor(_float4 vColor) { m_tParams.vDstColor = vColor; }

	_wstring Get_Text() { return m_szText; }
	void Set_Text(const _tchar* szText) { lstrcpy(m_szText, szText); }
	void Set_Text(_float fValue);
	
	void Shake(_float2 vOffset);
	void Set_Move(_float2 vMoveDir, _float fSpeed, _float fMaxDistance);
	void Move(_float fTimeDelta);

	void Start_ScalingY(_float fStartScale, _float fEndScale);
	void ScalingY(_float fTimeDelta);

	void Set_ObjectLocate(CGameObject* pGameObject, _float2 vOffset);
	_float2 Get_ObjectLocate(CGameObject* pGameObject, _float2 vOffset);

	void Set_ShaderPassNum(_uint iPassNum) { m_iShaderPass = iPassNum; }

	void Change_TextColor(_float4 vColor) { m_vTextColor = vColor; }
	void Change_HovorTextColor(_float4 vColor) { m_vHoverTextColor = vColor; }

protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pMaskTextureCom = { nullptr };
	CTexture* m_pCoverTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	UI_PARAMS			m_tParams = {};

	_float2				m_vScale = { g_iWindowSizeX, g_iWindowSizeY };
	_float3				m_vLocate = { g_iWindowSizeX * 0.5f, g_iWindowSizeY * 0.5f, 0.f };

	_float3				m_vAxis = { 0.f, 0.f, 1.f };
	_float				m_fDegrees = {};

	_tchar				m_szText[MAX_PATH] = {};
	_float2				m_vTextOffset = {};
	_float				m_fTextScale = {};
	_float4				m_vTextColor = { 0.f, 0.f, 0.f, 1.f };
	_float4				m_vHoverTextColor = { 1.f, 1.f, 1.f, 1.f };

	_wstring			m_strTextureTag = L"";
	_wstring			m_strFontTag = L"";
	_wstring			m_strMaskTextureTag = L"";

	_uint				m_iShaderPass = { 0 };

protected:
	_bool				m_bInWorld = { false };
	
	_float				m_fStretchFactor = { 1.0f };
	_float				m_fWorldOffsetY = {0.f};
	_uint				m_iTextureNum = {};
	_uint				m_iCoverTextureNum = {};
	_float				m_fInterval = {};
	_bool				m_bHover = { false }; // ¸¶¿ì½º
	_bool				m_bHovered = { false };
	_bool				m_bRightAngle = { false };
	_bool				m_bStretchY = { false };

	_bool				m_bScaling = { false };
	_float				m_fScalingRatio = { 1.f };
	_float				m_fStartScale = { 0.f };
	_float				m_fEndScale = { 0.f };

	_bool				m_bMoving = { false };
	_float				m_fMoveTime = { 0.f };
	_float				m_fMoveDistance = { 0.f };

	_float2				m_vMoveDir = {};
	_float				m_fSpeed = {};
	_float				m_fMaxDistance = {};

	SORTING				m_eSorting = {};
	CUIObject*			m_pOwner = { nullptr };
protected:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components(DESC& Desc);

	void Change_World(_float fOffsetY);
	_wstring Format_With_Comma(_float fValue);

public:
	static CUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
