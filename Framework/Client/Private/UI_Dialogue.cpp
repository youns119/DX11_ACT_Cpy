#include "pch.h"
#include "UI_Dialogue.h"

#include "GameInstance.h"
#include "Player.h"
#include "FileData_Locator.h"
#include "FData.h"
#include "Weapon_Player.h"


#include "UI_Speaker.h"
#include "UI_DialogueBox.h"

CUI_Dialogue::CUI_Dialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Dialogue::CUI_Dialogue(const CUI_Dialogue& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Dialogue::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Dialogue::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_isActive = false;

	Set_TextureTag();

	return S_OK;
}

void CUI_Dialogue::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Dialogue::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_vecSize = m_vecCurDialogueData.size();

	if (m_vecSize > 0)
	{
		if (m_iCurIndex == -1)
		{
			Proceed_Dialogue();
		}
		else
		{
			if (m_pGameInstance->Key_Down(KEY::ENTER))
			{
				Proceed_Dialogue();
			}
		}
	}
	super::Update(fTimeDelta);
}

void CUI_Dialogue::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Dialogue::Render()
{
    return S_OK;
}

void CUI_Dialogue::Set_Dialogue(_int iFileNum, _int iCurState, std::function<void()> Callback)
{
	m_vecCurDialogueData.clear();

	vector<FDialogueData> vecAllData = CFileData_Locator<FDialogueData>::Get_FileData()->Get_Datas(iFileNum);
		
	for (_int i = 0; i < vecAllData.size(); i++)
	{
		if (vecAllData[i].iStateNum == iCurState)
		{
			// 블랙스미스 상점이라면
			if (iFileNum == 6 && iCurState == 1)
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
				_int iCurWeaponLevel = pPlayer->Get_PlayerWeapon()->Get_WeaponStat_Const()->iLevel;

				_tchar szBuffer[256];
				swprintf_s(szBuffer, L"스테인리스 유물 %d개 줘라 고철.", iCurWeaponLevel);

				lstrcpy(vecAllData[i].szDialogue, szBuffer);
			}
			m_vecCurDialogueData.push_back(vecAllData[i]);
		}
	}

	m_iCurIndex = -1;
	m_Callback = Callback;
}

void CUI_Dialogue::Proceed_Dialogue()
{
	m_iCurIndex++;

	if (m_iCurIndex < m_vecSize)
	{
		m_pGameInstance->Play_Sound(L"Dialogue.wav", (_uint)SOUND_CHANNEL::MAPOBJECT, 0.6f);

		m_tData = m_vecCurDialogueData[m_iCurIndex];

		_wstring strPartTag = L"";

		if (wcscmp(m_tData.szSpeaker, L"PLAYER") == 0)
		{
			m_pUISpeakerKril->Set_IsActive(true);
			m_pUISpeakerKril->Change_TextureNumber(m_tData.iFaceTextureNum);
			m_pUISpeakerKril->Set_Quiet(false);
			m_pUISpeakerOther->Set_Quiet(true);
			m_pUIBoxLeft->Set_IsActive(true);
			m_pUIBoxLeft->Set_DialogueText(m_tData.szDialogue);
			m_pUIBoxRight->Set_IsActive(false);
		}
		else
		{
			m_pUISpeakerOther->Set_IsActive(true);
			m_pUISpeakerOther->Change_Texture(m_mapTextureTag[m_tData.szSpeaker], m_tData.iFaceTextureNum);
			m_pUISpeakerKril->Set_Quiet(true);
			m_pUISpeakerOther->Set_Quiet(false);
			m_pUIBoxLeft->Set_IsActive(false);
			m_pUIBoxRight->Set_IsActive(true);
			m_pUIBoxRight->Set_DialogueText(m_tData.szDialogue);
		}
	}
	else
	{
		m_iCurIndex = -1;
		m_isActive = false;

		m_pUISpeakerKril->Set_IsActive(false);
		m_pUISpeakerOther->Set_IsActive(false);

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
		pPlayer->Get_PlayerStat()->bInputEnable = true;

		if(m_Callback)
			m_Callback();
	}
}

void CUI_Dialogue::Set_TextureTag()
{
	m_mapTextureTag[L"PLAYER"] = L"Prototype_Component_Texture_Kril_dialogue";
	m_mapTextureTag[L"NPC_CRAB"] = L"Prototype_Component_Texture_NPC_Crab_dialogue";
	m_mapTextureTag[L"NPC_HERMIT"] = L"Prototype_Component_Texture_NPC_Hermit_dialogue";

	m_mapTextureTag[L"NPC_SHARK"] = L"Prototype_Component_Texture_NPC_Shark_dialogue";

	m_mapTextureTag[L"NPC_BABLO"] = L"Prototype_Component_Texture_NPC_Bablo_dialogue";
	m_mapTextureTag[L"NPC_CHILDREN"] = L"Prototype_Component_Texture_NPC_Children_dialogue";
	m_mapTextureTag[L"NPC_ROLAND"] = L"Prototype_Component_Texture_NPC_Roland_dialogue";
	m_mapTextureTag[L"NPC_TORTELLINI"] = L"Prototype_Component_Texture_NPC_Tortellini_dialogue";
	m_mapTextureTag[L"NPC_URCHIN"] = L"Prototype_Component_Texture_NPC_Urchin_dialogue";

	m_mapTextureTag[L"NPC_BLACKSMITH"] = L"Prototype_Component_Texture_NPC_BlackSmith_dialogue";
	m_mapTextureTag[L"NPC_CHITAN"] = L"Prototype_Component_Texture_NPC_Chitan_dialogue";
	m_mapTextureTag[L"NPC_NEMMA"] = L"Prototype_Component_Texture_NPC_Nemma_dialogue";
	m_mapTextureTag[L"NPC_PRAWNATHAN"] = L"Prototype_Component_Texture_NPC_Prawnathan_dialogue";
	m_mapTextureTag[L"NPC_TOPODA"] = L"Prototype_Component_Texture_NPC_Topoda_dialogue";

	m_mapTextureTag[L"NPC_FIRTH"] = L"Prototype_Component_Texture_NPC_Firth_dialogue";
}

HRESULT CUI_Dialogue::Ready_PartObject()
{
	/* Dialogue_Box_Left */
	CUI_DialogueBox::DESC BoxDesc{};
	{
		BoxDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Dialogue/UI_Dialogue_Box_Left.dat";
		BoxDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Dialogue_Box_Left", LEVEL_STATIC, L"Prototype_GameObject_UI_DialogueBox", &BoxDesc)))
		return E_FAIL;

	m_pUIBoxLeft = static_cast<CUI_DialogueBox*>(Find_PartObject({ L"Part_UI_Dialogue_Box_Left" }));

	/* Dialogue_Box_Right */
	{
		BoxDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Dialogue/UI_Dialogue_Box_Right.dat";
		BoxDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Dialogue_Box_Right", LEVEL_STATIC, L"Prototype_GameObject_UI_DialogueBox", &BoxDesc)))
		return E_FAIL;

	m_pUIBoxRight = static_cast<CUI_DialogueBox*>(Find_PartObject({ L"Part_UI_Dialogue_Box_Right" }));

	/* Speaker_Kril */
	CUI_Speaker::DESC SpeakerDesc{};
	{
		SpeakerDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Dialogue/UI_Dialogue_Kril.dat";
		SpeakerDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Speaker_Kril", LEVEL_STATIC, L"Prototype_GameObject_UI_Speaker", &SpeakerDesc)))
		return E_FAIL;

	m_pUISpeakerKril = static_cast<CUI_Speaker*>(Find_PartObject({ L"Part_UI_Speaker_Kril" }));

	/* Part_UI_Speaker_Other */
	{
		SpeakerDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Dialogue/UI_Dialogue_Other.dat";
		SpeakerDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Speaker_Other", LEVEL_STATIC, L"Prototype_GameObject_UI_Speaker", &SpeakerDesc)))
		return E_FAIL;

	m_pUISpeakerOther = static_cast<CUI_Speaker*>(Find_PartObject({ L"Part_UI_Speaker_Other" }));

	return S_OK;
}

CUI_Dialogue* CUI_Dialogue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Dialogue* pInstance = new CUI_Dialogue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Dialogue::Clone(void* pArg)
{
	CUI_Dialogue* pInstance = new CUI_Dialogue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Dialogue::Free()
{
    CUIObject::Free();
}

