#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_DynamicTexture;

class CUI_ShopButton final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float2 vLocate = {};
		_float fStretchFactor = { 0.f };
	};

private:
	CUI_ShopButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ShopButton(const CUI_ShopButton& Prototype);
	virtual ~CUI_ShopButton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_Callback(std::function<void()> callback) { m_Callback = callback; }
	void Set_HoverCallback(std::function<void()> callback) { m_HoverCallback = callback; }

	void Change_ItemTexture(_wstring strTextureTag, _int iTextureNum);
	void Set_ItemData(_int iItemCount, const _tchar* szItemName, _int iItemPrice);
private:
	CUI_DynamicTexture* m_pUIItemTexture = {nullptr};

	std::function<void()> m_Callback;
	std::function<void()> m_HoverCallback;

	_float m_fFrame = {};
	_float m_fRatio = {};

	_tchar m_szItemCountText[MAX_PATH] = L"";
	_tchar m_szItemName[MAX_PATH] = L"";
	_tchar m_szItemPrice[MAX_PATH] = L"";

private:
	HRESULT Ready_PartObject();

public:
	static CUI_ShopButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
