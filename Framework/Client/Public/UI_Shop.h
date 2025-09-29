#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CUI_DefaultStretch;
class CUI_ShopButton;
class CUI_Shake;
class CUI_Description;

class CInventory;

class CUI_Shop final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Shop(const CUI_Shop& Prototype);
	virtual ~CUI_Shop() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_SellShop(_bool bIsSellShop) { m_bIsSellShop = bIsSellShop; }
	void Set_Inventory(CInventory* pFromInventory, CInventory* pToInventory = nullptr)
	{ 
		m_pFromInventory = pFromInventory;
		m_pToInventory = pToInventory;
	}

private:
	CUI_DefaultStretch* m_pUIBorder = { nullptr };
	CUI_ShopButton* m_pUIButton[8] = { nullptr };
	CUI_Shake* m_pUIDeco[2] = { nullptr };
	CUI_Description* m_pUIDesc;

	CInventory* m_pFromInventory = { nullptr };
	CInventory* m_pToInventory = { nullptr };

	_bool		m_bIsSellShop = { false };

	_float4		m_vBuyColor = { 242.f / 255.f, 128.f / 255.f, 102.f / 255.f, 1.f };
	_float4		m_vSellColor = { 118.f / 255.f, 180.f / 255.f, 96.f / 255.f, 1.f };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Shop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
