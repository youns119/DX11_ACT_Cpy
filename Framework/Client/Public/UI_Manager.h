#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "FPlayerStat.h"
#include "FShell.h"
#include "UI_Menu.h"
#include "UI_Status.h"
#include "UI_Inventory.h"
#include "ShellHouse.h"
#include "Item_Trash.h"

BEGIN(Client)

class CInventory;

class CUI_CutScene;

class CUI_Button;
class CUI_CopyTexture;
class CUI_Fade;
class CUI_Mouse;
class CUI_Text;
class CUI_TextTexture;

class CUI_Dialogue;

class CUI_Notice;
class CUI_Interaction;
 
class CUI_MoonSnail;
class CUI_Sheleport;
class CUI_Stat;
 
class CUI_Shop;
class CUI_ShopSelect;

class CUI_GainSkill;
class CUI_Texture;
class CUI_Credit;

class CUI_Manager final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Manager(const CUI_Manager& Prototype);
	virtual ~CUI_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	HRESULT Add_PartObject_For_Static();
	HRESULT Add_PartObject_For_Play();

public:
	void Set_PlayerStat(FPlayerStat* pPlayerStat) { m_pPlayerStat = pPlayerStat; }
	FPlayerStat* Get_PlayerStat() { return m_pPlayerStat; }

	void Set_PlayerShellStat_Ptr(FShellStat** ppShellStat) { m_ppPlayerShellStat = ppShellStat; }
	FShellStat** Get_PlayerShellStat_Ptr() { return m_ppPlayerShellStat; }

	void Set_MenuUI(CGameObject* pUIMenu) {m_pUIMenu = static_cast<CUI_Menu*>(pUIMenu);}
	void Set_StatusUI(CGameObject* pUIMenu) {m_pUIStatus = static_cast<CUI_Status*>(pUIMenu);}
	void Set_InventoryUI(CGameObject* pUIMenu) {m_pUIItemInventory = static_cast<CUI_Inventory*>(pUIMenu);}

	_bool Get_IsActive_MenuUI() { return m_pUIMenu->Get_IsActive(); }
public:
	void Set_Micro(_uint iMicroplastic) { m_iMicroplasticCount = iMicroplastic; }
	_uint& Get_Micro_Ref() {return m_iMicroplasticCount;}

	CInventory* Get_ItemInventory() { return m_pItemInventory; }
	CInventory* Get_ShopItemInventory() { return m_pShopItemInventory; }
	CInventory* Get_QuestItemInventory() { return m_pQuestItemInventory; }
	
	_bool Get_CopyTexture_IsActive();
	_bool Get_TextTexture_IsActive();

	void Show_TextUI(const _tchar* szText, _float4 vColor = { 1.f, 1.f, 1.f, 1.f }, _bool bShowEffect = true);

	void FadeIn();
	void FadeOut();

	_bool End_FadeIn();
	_bool End_FadeOut();

	void Set_FadeSpeed(_float _Speed);

	void Show_Mouse();
	void Hide_Mouse();
	_bool Get_IsActive_Mouse();

	void Show_CutSceneUI();
	void Hide_CutSceneUI();
	_bool Get_CutSceneUI_IsActive();
	void Set_CutSceneUI_Caption(const _tchar* szCaption);

	_bool Get_DialogueUI_IsActive();
	void Set_DialogueUI(_int iFileNum, _int iCurState, std::function<void()> Callback = {});
	_int Get_CurDialogueIndex();

	void Show_MenuUI();
	void Hide_MenuUI();

	void Show_BuyShopUI();
	void Show_SellShopUI();
	void Hide_ShopUI(std::function<void()> AfterHideCallback = {});
	_bool Get_ShopUI_IsActvie();

	void Show_ShopSelectUI();
	void Hide_ShopSelectUI(std::function<void()> AfterHideCallback = {});
	_bool Get_ShopSelectUI_IsActive();

	void Show_Interaction(const _tchar* szText);
	void Hide_Interaction();

	void Show_Caption(const _tchar* szText, _float fCaptionTime);
	void Hide_Caption();

	void Add_Item(CItem_Trash* pItem);
	void Add_QuestItem(CItem_Trash* pItem);
	void Show_Notice(CItem_Trash* pItem);

	void Set_MoonSnailUI(CShellHouse::DATA data);
	void Show_MoonSnailUI();
	void Hide_MoonSnailUI(_bool bIsAnim = true);

	void Add_Visited(CShellHouse::DATA data);
	void Show_SheleportUI();
	void Hide_SheleportUI(std::function<void()> AfterHideCallback = {});

	void Show_StatUI();
	void Hide_StatUI(std::function<void()> AfterHideCallback = {});

	void Set_MicroPlastic(_int iValue);
	_uint Get_StainlessRelic() { return m_pShopItemInventory->Get_ItemCount(0);}

	void Show_GainActiveSkillUI(_int iSkillTextureNum, std::function<void()> callback = {});
	void Show_GainPassiveSkillUI(_int iSkillTextureNum, std::function<void()> callback = {});
	void Hide_GainSkillUI();

	void Show_CreditUI();
	_bool Get_IsActive_CreditUI();

private:
	FPlayerStat* m_pPlayerStat = { nullptr };
	FShellStat** m_ppPlayerShellStat = { nullptr };

	_bool		m_bReady = { false };
	_uint		m_iMicroplasticCount = {0};

	_float		m_fCaptionTime = {};

	// PlayerInventory
	CInventory* m_pItemInventory = { nullptr };
	CInventory* m_pShopItemInventory = { nullptr };
	CInventory* m_pQuestItemInventory = { nullptr };

	// NPCInventory
	CInventory* m_pAppleKingInventory = { nullptr };
	CInventory* m_pNemmaInventory = { nullptr };
	CInventory* m_pPrawnathanInventory = { nullptr };

private:
	CUI_Menu* m_pUIMenu = { nullptr };
	CUI_Inventory* m_pUIItemInventory = { nullptr };
	CUI_Status* m_pUIStatus = { nullptr };

private:
	CUI_CopyTexture* m_pUICopyTexture = { nullptr };
	CUI_TextTexture* m_pUITextTexture = { nullptr };

	CUI_Fade* m_pUIFade = {nullptr};
	CUI_Text* m_pUIText = { nullptr };
	CUI_Mouse* m_pUIMouse = { nullptr };
	CUI_CutScene* m_pUICutScene = { nullptr };

	CUI_Dialogue* m_pUIDialogue = { nullptr };

	CUI_Notice* m_pUINotice = { nullptr };
	CUI_Interaction* m_pUIInteraction = { nullptr };
	CUI_Texture* m_pUICaption = { nullptr };

	CUI_MoonSnail* m_pUIMoonSnail = { nullptr };
	CUI_Sheleport* m_pUISheleport = { nullptr };
	CUI_Stat* m_pUIStat = { nullptr };

	CUI_Shop* m_pUIShop = { nullptr };
	CUI_ShopSelect* m_pUIShopSelect = { nullptr };

	CUI_GainSkill* m_pUIGainSkill = { nullptr };
	CUI_Credit* m_pUICredit = { nullptr };

public:
	static CUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
