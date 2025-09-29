#include "pch.h"
#include "UI_Manager.h"
#include "Player.h"
#include "FData.h"

#include "GameInstance.h"
#include "Inventory.h"
#include "UI_Headers.h"

CUI_Manager::CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CUI_Manager::CUI_Manager(const CUI_Manager& Prototype)
	: super(Prototype)
{
}

HRESULT CUI_Manager::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::Initialize(void* pArg)
{
	CUI_Manager::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}	

void CUI_Manager::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Manager::Update(_float fTimeDelta)
{		
	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVELID::LEVEL_LOADING)
	{
		if (m_pUIMenu)
		{
			const _bool bIsOtherUIActive =
				m_pUIMoonSnail->Get_IsActive() ||
				m_pUISheleport->Get_IsActive() ||
				m_pUIStat->Get_IsActive() ||
				m_pUIShopSelect->Get_IsActive() ||
				m_pUIShop->Get_IsActive();

			if (!bIsOtherUIActive && m_pGameInstance->Key_Down(KEY::ESCAPE))
			{
				if (m_pUIMenu->Get_IsActive())
					Hide_MenuUI();
				else
					Show_MenuUI();
			}
		}
	}
	
	if (m_pUICaption)
	{
		if (m_pUICaption->Get_IsActive())
		{
			m_fCaptionTime -= fTimeDelta;

			if (m_fCaptionTime <= 0.f)
			{
				Hide_Caption();
			}
		}
	}

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
	
	if (pPlayer)
	{
		pPlayer->Get_PlayerStat()->iHookCount = m_pShopItemInventory->Get_ItemCount(2);
	}

	if (m_pGameInstance->Key_Down(KEY::UP))
	{
		m_iMicroplasticCount += 5;
		m_pShopItemInventory->Add(2, 5);
	}
	if (m_pGameInstance->Key_Down(KEY::DOWN))
		m_iMicroplasticCount -= 5;
	if (m_pGameInstance->Key_Down(KEY::LEFT))
		m_iMicroplasticCount -= 100;
	if (m_pGameInstance->Key_Down(KEY::RIGHT))
		m_iMicroplasticCount += 100;

	super::Update(fTimeDelta);
}

void CUI_Manager::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

_bool CUI_Manager::Get_CopyTexture_IsActive()
{
	return m_pUICopyTexture->Get_IsActive();
}

_bool CUI_Manager::Get_TextTexture_IsActive()
{
	return m_pUITextTexture->Get_IsActive();
}

_bool CUI_Manager::Get_DialogueUI_IsActive()
{
	return m_pUIDialogue->Get_IsActive();
}

void CUI_Manager::Show_TextUI(const _tchar* szText, _float4 vColor, _bool bShowEffect)
{
	m_pUITextTexture->Show(bShowEffect);
	m_pUIText->Set_Text(szText);
	m_pUIText->Change_TextColor(vColor);
	m_pUIText->Set_IsActive(true);
}

void CUI_Manager::FadeIn()
{
	m_pUIFade->FadeIn();
}

void CUI_Manager::FadeOut()
{
	m_pUIFade->FadeOut();
}

_bool CUI_Manager::End_FadeIn()
{
	return m_pUIFade->End_FadeIn();
}

_bool CUI_Manager::End_FadeOut()
{
	return m_pUIFade->End_FadeOut();
}

void CUI_Manager::Set_FadeSpeed(_float _fSpeed)
{
	return m_pUIFade->Set_Speed(_fSpeed);
}

void CUI_Manager::Show_Mouse()
{
	if (m_pUIMouse->Get_IsActive()) return;

	m_pUIMouse->Set_IsActive(true);
	
	ShowCursor(TRUE);
}

void CUI_Manager::Hide_Mouse()
{
	if (!m_pUIMouse->Get_IsActive()) return;

	m_pUIMouse->Set_IsActive(false);

	ShowCursor(FALSE);
}

_bool CUI_Manager::Get_IsActive_Mouse()
{
	return m_pUIMouse->Get_IsActive();
}

void CUI_Manager::Show_CutSceneUI()
{
	m_pUICutScene->Set_IsActive(true);
}

void CUI_Manager::Hide_CutSceneUI()
{
	m_pUICutScene->Set_Text(L"");
	m_pUICutScene->Set_IsActive(false);
}

_bool CUI_Manager::Get_CutSceneUI_IsActive()
{
	return m_pUICutScene->Get_IsActive();
}

void CUI_Manager::Set_CutSceneUI_Caption(const _tchar* szCaption)
{
	m_pUICutScene->Set_Text(szCaption);
}

void CUI_Manager::Set_DialogueUI(_int iFileNum, _int iCurState, std::function<void()> Callback)
{
	m_pUIDialogue->Set_Dialogue(iFileNum, iCurState, Callback);
	m_pUIDialogue->Set_IsActive(true);

	m_pPlayerStat->bInputEnable = false;
}

_int CUI_Manager::Get_CurDialogueIndex()
{
	return m_pUIDialogue->Get_CurDialogueIndex();
}

void CUI_Manager::Show_MenuUI()
{
	m_pGameInstance->Play_Sound(L"Menu_Pause.wav", (_uint)SOUND_CHANNEL::UI, 1.f);
	m_pGameInstance->Set_UIBlur(true);
	m_pUICopyTexture->Show();
	m_pUIMenu->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Hide_MenuUI()
{
	m_pGameInstance->Play_Sound(L"UI Unpause.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);

	m_pUICopyTexture->Hide();
	m_pUICopyTexture->Set_HideCallback([this]()->void
		{
			m_pUIMenu->Set_IsActive(false);
			Hide_Mouse();
			m_pGameInstance->Set_UIBlur(false);
		});
}

void CUI_Manager::Show_BuyShopUI()
{
	m_pUIShop->Set_SellShop(false);
	m_pUIShop->Set_Inventory(m_pPrawnathanInventory, m_pShopItemInventory);

	m_pUIShop->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Show_SellShopUI()
{
	m_pGameInstance->Set_UIBlur(true);
	m_pUICopyTexture->Show();
	m_pUIShop->Set_SellShop(true);
	m_pUIShop->Set_Inventory(m_pItemInventory);
	m_pUIShop->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Hide_ShopUI(std::function<void()> AfterHideCallback)
{
	m_pUICopyTexture->Hide();
	m_pUICopyTexture->Set_HideCallback([this, AfterHideCallback]()->void
		{
			m_pUIShop->Set_IsActive(false);
			Hide_Mouse();
			m_pGameInstance->Set_UIBlur(false);

			if (AfterHideCallback)
				AfterHideCallback();
		});
}

_bool CUI_Manager::Get_ShopUI_IsActvie()
{
	return m_pUIShop->Get_IsActive();
}

void CUI_Manager::Show_ShopSelectUI()
{
	m_pGameInstance->Set_UIBlur(true);
	m_pUICopyTexture->Show();
	m_pUIShopSelect->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Hide_ShopSelectUI(std::function<void()> AfterHideCallback)
{
	m_pUICopyTexture->Hide();
	m_pUICopyTexture->Set_HideCallback([this, AfterHideCallback]()->void
		{
			m_pUIShopSelect->Set_IsActive(false);
			Hide_Mouse();
			m_pGameInstance->Set_UIBlur(false);

			if (AfterHideCallback)
				AfterHideCallback();
		});
}

_bool CUI_Manager::Get_ShopSelectUI_IsActive()
{
	return m_pUIShopSelect->Get_IsActive();
}

void CUI_Manager::Show_Interaction(const _tchar* szText)
{
	m_pUIInteraction->Set_Text(szText);
	m_pUIInteraction->Show();
}

void CUI_Manager::Hide_Interaction()
{
	if(m_pUIInteraction != nullptr)
		m_pUIInteraction->Hide();
}

void CUI_Manager::Show_Caption(const _tchar* szText, _float fCaptionTime)
{
	m_fCaptionTime = fCaptionTime;
	m_pUICaption->Set_Text(szText);
	m_pUICaption->Set_IsActive(true);
}

void CUI_Manager::Hide_Caption()
{
	m_pUICaption->Set_IsActive(false);
}

void CUI_Manager::Add_Item(CItem_Trash* pItem)
{
	m_pItemInventory->Add(pItem);
	Hide_Interaction();
	Show_Notice(pItem);
}

void CUI_Manager::Show_Notice(CItem_Trash* pItem)
{
	m_pUINotice->Show(pItem);
}

void CUI_Manager::Set_MoonSnailUI(CShellHouse::DATA data)
{
	m_pUIMoonSnail->Set_Data(data.SheleportData.szLocation, data.SheleportData.szName);
}

void CUI_Manager::Show_MoonSnailUI()
{
	//m_pUICopyTexture->Show();
	m_pUIMoonSnail->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Hide_MoonSnailUI(_bool bIsAnim)
{
	//m_pGameInstance->Play_Sound(L"MSS Exit.wav", (_uint)SOUND_CHANNEL::UI, 1.f);
	m_pGameInstance->Play_Sound(L"MSS_Exit_Water.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);

	if (bIsAnim)
	{
		m_pUICopyTexture->Hide();
		m_pUICopyTexture->Set_HideCallback([this]()->void
			{
				m_pUIMoonSnail->Set_IsActive(false);
				Hide_Mouse();
				m_pGameInstance->Set_UIBlur(false);
			});
	}
	else
	{
		m_pUIMoonSnail->Set_IsActive(false);
		Hide_Mouse();
		m_pGameInstance->Set_UIBlur(false);
	}
}

void CUI_Manager::Add_Visited(CShellHouse::DATA data)
{
	m_pUISheleport->Add_Visited(data);
}

void CUI_Manager::Show_SheleportUI()
{
	m_pGameInstance->Set_UIBlur(true);
	m_pUICopyTexture->Show();
	m_pUISheleport->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Hide_SheleportUI(std::function<void()> AfterHideCallback)
{
	m_pUICopyTexture->Hide();
	m_pUICopyTexture->Set_HideCallback([this, AfterHideCallback]()->void
		{
			m_pUISheleport->Set_IsActive(false);
			Hide_Mouse();

			if (AfterHideCallback)
				AfterHideCallback();
		});
	m_pGameInstance->Set_UIBlur(false);
}

void CUI_Manager::Show_StatUI()
{
	m_pUICopyTexture->Show();
	m_pUIStat->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Hide_StatUI(std::function<void()> AfterHideCallback)
{
	m_pUICopyTexture->Hide();
	m_pUICopyTexture->Set_HideCallback([this, AfterHideCallback]()->void
		{
			m_pUIStat->Set_IsActive(false);
			Hide_Mouse();

			if (AfterHideCallback)
				AfterHideCallback();
		});
}

void CUI_Manager::Set_MicroPlastic(_int iValue)
{
	m_iMicroplasticCount += iValue;
}

void CUI_Manager::Show_GainActiveSkillUI(_int iSkillTextureNum, std::function<void()> callback)
{
	m_pGameInstance->Play_Sound(L"SkillGet.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);

	m_pUICopyTexture->Show();
	m_pUIGainSkill->Set_ActiveSkill(iSkillTextureNum, callback);
	m_pUIGainSkill->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Show_GainPassiveSkillUI(_int iSkillTextureNum, std::function<void()> callback)
{
	m_pGameInstance->Play_Sound(L"SkillGet.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);

	m_pUICopyTexture->Show();
	m_pUIGainSkill->Set_PassiveSkill(iSkillTextureNum, callback);
	m_pUIGainSkill->Set_IsActive(true);
	Show_Mouse();
}

void CUI_Manager::Hide_GainSkillUI()
{
	m_pUICopyTexture->Hide();
	m_pUICopyTexture->Set_HideCallback([this]()->void
		{
			m_pUIGainSkill->Set_IsActive(false);
			Hide_Mouse();
		});
}

void CUI_Manager::Show_CreditUI()
{
	m_pUICredit->Set_IsActive(true);
}

_bool CUI_Manager::Get_IsActive_CreditUI()
{
	return m_pUICredit->Get_IsActive();
}

HRESULT CUI_Manager::Add_PartObject_For_Static()
{
	CUI_CopyTexture::DESC CopyTextureDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"CopyTexture", LEVEL_STATIC, L"Prototype_GameObject_UI_CopyTexture", &CopyTextureDesc)))
		return E_FAIL;

	m_pUICopyTexture = static_cast<CUI_CopyTexture*>(Find_PartObject({ L"CopyTexture" }));

	CUI_TextTexture::DESC TextTextureDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"TextTexture", LEVEL_STATIC, L"Prototype_GameObject_UI_TextTexture", &TextTextureDesc)))
		return E_FAIL;

	m_pUITextTexture = static_cast<CUI_TextTexture*>(Find_PartObject({ L"TextTexture" }));

	CUI_Text::DESC TextDesc{};
	{
		TextDesc.UIDesc.eSorting = SORTING::MIDDLE;
		TextDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Text.dat";
	}
	if (FAILED(Add_PartObject(L"Text", LEVEL_STATIC, L"Prototype_GameObject_UI_Text", &TextDesc)))
		return E_FAIL;

	m_pUIText = static_cast<CUI_Text*>(Find_PartObject({ L"Text" }));

	CUI_Fade::DESC FadeDesc{};
	{
		FadeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Fade.dat";
	}
	if (FAILED(Add_PartObject(L"Fade", LEVEL_STATIC, L"Prototype_GameObject_UI_Fade", &FadeDesc)))
		return E_FAIL;

	m_pUIFade = static_cast<CUI_Fade*>(Find_PartObject({ L"Fade" }));
	
	CUI_Mouse::DESC MouseDesc{};
	{
		MouseDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Cursor.dat";
	}
	if (FAILED(Add_PartObject(L"Mouse", LEVEL_STATIC, L"Prototype_GameObject_UI_Mouse", &MouseDesc)))
		return E_FAIL;

	m_pUIMouse = static_cast<CUI_Mouse*>(Find_PartObject({ L"Mouse" }));

	m_pItemInventory = CInventory::Create(CInventory::INVEN_TRASH);
	m_pShopItemInventory = CInventory::Create(CInventory::INVEN_SHOP);
	m_pQuestItemInventory = CInventory::Create(CInventory::INVEN_QUEST);

	// 갈고리 처음에 5개 제공
	m_pShopItemInventory->Add(2, 5);

	m_pQuestItemInventory->Add(0, 1);

	m_pPrawnathanInventory = CInventory::Create(CInventory::INVEN_SHOP);

	m_pQuestItemInventory->Add(0, 1);

	// 새우 상점 인벤토리
	m_pPrawnathanInventory->Add(0, 3);
	m_pPrawnathanInventory->Add(1, 5);
	m_pPrawnathanInventory->Add(2, 5);
	m_pPrawnathanInventory->Add(3, 5);
	m_pPrawnathanInventory->Add(4, 1);

	CUI_CutScene::DESC CutSceneDesc{};
	{
		CutSceneDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_LockOn.dat";
	}
	if (FAILED(Add_PartObject(L"CutScene", LEVEL_STATIC, L"Prototype_GameObject_UI_CutScene", &CutSceneDesc)))
		return E_FAIL;

	m_pUICutScene = static_cast<CUI_CutScene*>(Find_PartObject({ L"CutScene" }));

	return S_OK;
}

HRESULT CUI_Manager::Add_PartObject_For_Play()
{
	if (m_bReady)
		return S_OK;

	m_bReady = true;

	CUI_Dialogue::DESC DialogueDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"Dialogue", LEVEL_STATIC, L"Prototype_GameObject_UI_Dialogue", &DialogueDesc)))
		return E_FAIL;

	m_pUIDialogue = static_cast<CUI_Dialogue*>(Find_PartObject({ L"Dialogue" }));

	CUI_Notice::DESC NoticeDesc{};
	{
		NoticeDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Notice.dat";
	}
	if (FAILED(Add_PartObject(L"Notice", LEVEL_STATIC, L"Prototype_GameObject_UI_Notice", &NoticeDesc)))
		return E_FAIL;

	m_pUINotice = static_cast<CUI_Notice*>(Find_PartObject({ L"Notice" }));

	CUI_Interaction::DESC InteractionDesc{};
	{
		InteractionDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_InteractionBorder.dat";
	}
	if (FAILED(Add_PartObject(L"Interaction", LEVEL_STATIC, L"Prototype_GameObject_UI_Interaction", &InteractionDesc)))
		return E_FAIL;

	m_pUIInteraction = static_cast<CUI_Interaction*>(Find_PartObject({ L"Interaction" }));

	CUI_Texture::DESC CaptionDesc{};
	{
		CaptionDesc.UIDesc.eSorting = SORTING::MIDDLE;
		CaptionDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Caption.dat";
		CaptionDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Caption", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &CaptionDesc)))
		return E_FAIL;

	m_pUICaption = static_cast<CUI_Texture*>(Find_PartObject({ L"Caption" }));

	CUI_MoonSnail::DESC MoonSnailDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"MoonSnail", LEVEL_STATIC, L"Prototype_GameObject_UI_MoonSnail", &MoonSnailDesc)))
		return E_FAIL;

	m_pUIMoonSnail = static_cast<CUI_MoonSnail*>(Find_PartObject({ L"MoonSnail" }));

	CUI_Sheleport::DESC SheleportDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"Sheleport", LEVEL_STATIC, L"Prototype_GameObject_UI_Sheleport", &SheleportDesc)))
		return E_FAIL;

	m_pUISheleport = static_cast<CUI_Sheleport*>(Find_PartObject({ L"Sheleport" }));

	CUI_Stat::DESC StatDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"Stat", LEVEL_STATIC, L"Prototype_GameObject_UI_Stat", &StatDesc)))
		return E_FAIL;

	m_pUIStat = static_cast<CUI_Stat*>(Find_PartObject({ L"Stat" }));


	CUI_Shop::DESC ShopDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"Shop", LEVEL_STATIC, L"Prototype_GameObject_UI_Shop", &ShopDesc)))
		return E_FAIL;

	m_pUIShop = static_cast<CUI_Shop*>(Find_PartObject({ L"Shop" }));

	CUI_ShopSelect::DESC ShopSelectDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"ShopSelect", LEVEL_STATIC, L"Prototype_GameObject_UI_ShopSelect", &ShopSelectDesc)))
		return E_FAIL;

	m_pUIShopSelect = static_cast<CUI_ShopSelect*>(Find_PartObject({ L"ShopSelect" }));

	CUI_GainSkill::DESC SkillDesc{};
	{

	}
	if (FAILED(Add_PartObject(L"Skill", LEVEL_STATIC, L"Prototype_GameObject_UI_GainSkill", &SkillDesc)))
		return E_FAIL;

	m_pUIGainSkill = static_cast<CUI_GainSkill*>(Find_PartObject({ L"Skill" }));

	CUI_Credit::DESC CreditDesc{};
	{
		CreditDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Credit.dat";
		CreditDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Credit", LEVEL_STATIC, L"Prototype_GameObject_UI_Credit", &CreditDesc)))
		return E_FAIL;

	m_pUICredit = static_cast<CUI_Credit*>(Find_PartObject({ L"Credit" }));


	return S_OK;
}

CUI_Manager* CUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Manager* pInstance = new CUI_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Manager::Clone(void* pArg)
{
	CUI_Manager* pInstance = new CUI_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	super::Free();

	Safe_Release(m_pItemInventory);
	Safe_Release(m_pShopItemInventory);
	Safe_Release(m_pQuestItemInventory);

	Safe_Release(m_pAppleKingInventory);
	Safe_Release(m_pNemmaInventory);
	Safe_Release(m_pPrawnathanInventory);
}

