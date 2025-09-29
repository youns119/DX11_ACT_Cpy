#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "Inventory.h"

BEGIN(Client)

class CUI_Manager;
class CUI_Description;
class CUI_DynamicButton;
class CUI_DynamicTexture;

class CUI_Inventory final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		CUIObject* pOwner = { nullptr };
	};

private:
	CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Inventory(const CUI_Inventory& Prototype);
	virtual ~CUI_Inventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_Manager* m_pUIManager = { nullptr };
	CUIObject* m_pOwner = { nullptr };

	CInventory* m_pItemInventory = { nullptr };
	CInventory* m_pShopItemInventory = { nullptr };
	CInventory* m_pQuestItemInventory = { nullptr };

	_wstring m_strItemTextureTag = L"Prototype_Component_Texture_Item";
	_wstring m_strShopItemTextureTag = L"Prototype_Component_Texture_ShopItem";
	_wstring m_strQuestItemTextureTag = L"Prototype_Component_Texture_QuestItem";

	_float4	m_vColor = { 0.f, 0.f, 0.f, 1.f };

	CUI_Description* m_pUIDesc = { nullptr };
	CUI_DynamicButton* m_pUIInventory[15] = { nullptr };
	CUI_DynamicTexture* m_pUIItem[15] = { nullptr };
private:
	HRESULT Ready_PartObject();

public:
	static CUI_Inventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
