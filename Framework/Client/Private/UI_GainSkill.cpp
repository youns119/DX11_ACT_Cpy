#include "pch.h"
#include "UI_GainSkill.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "FileData_Locator.h"
#include "Player.h"

#include "UI_Texture.h"
#include "UI_DefaultStretch.h"
#include "UI_FixStretch.h"

CUI_GainSkill::CUI_GainSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_GainSkill::CUI_GainSkill(const CUI_GainSkill& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_GainSkill::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_GainSkill::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;
	pDesc->UIObjectDesc.fZ = 0.01f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	m_isActive = false;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CUI_GainSkill::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_GainSkill::Update(_float fTimeDelta)
{
	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	if (!m_isActive)
		return;

	if (m_fButtonMax <= m_fButtonCur)
	{
		if (!m_bIsRewarded)
		{
			if (m_bGainActive)
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
				pPlayer->UnLock_ActiveSkill((ACTIVESKILL)m_iSkillNum);
			}
			else
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
				pPlayer->UnLock_ActionSkill((ACTIONSKILL)m_iSkillNum);
			}
			
			m_pUIManager->Hide_GainSkillUI();
			m_bIsRewarded = true;
			m_fButtonCur = 0.f;

			if (m_Callback)
				m_Callback();

			m_pGameInstance->Play_Sound(L"AcceptSkill.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
		}
	}
	
	if(!m_bIsRewarded)
	{
		if (m_pGameInstance->Key_Press(KEY::E))
		{
			m_fButtonCur += 0.1f;
		}

		if (m_pGameInstance->Key_Up(KEY::E))
		{
			m_fButtonCur = 0.f;
			//m_pGameInstance->Stop_Sound((_uint)SOUND_CHANNEL::UI);
		}
	}

	super::Update(fTimeDelta);
}

void CUI_GainSkill::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_GainSkill::Render()
{
	Set_SkillText();

	_float2 vStart = { 640.f, 360.f };
	_float fOffsetY = { 20.f };

	for (_int i = 0; i < m_vecText.size(); i++)
	{
		_float2 vTextPos = { vStart.x, vStart.y + fOffsetY * i };
		m_pGameInstance->Render_Font(L"Font_Cafe_15", m_vecText[i].c_str(), vTextPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {0.f, 0.f}, 1.f, SORTING::MIDDLE);
	}

    return S_OK;
}

void CUI_GainSkill::Set_PassiveSkill(_int iTextureNum, std::function<void()> callback)
{
	m_iSkillNum = iTextureNum;

	FSkillData SkillData = CFileData_Locator<FSkillData>::Get_FileData()->Get_Data(1, iTextureNum);

	m_pUISkillTexture->Change_Texture(L"Prototype_Component_Texture_Technique", iTextureNum);
	m_pUISkillTexture->Set_Text(SkillData.szSkillName);
	lstrcpy(m_szText, SkillData.szSkillDescription);

	m_bGainActive = false;
	m_bIsRewarded = false;

	m_Callback = callback;
}

void CUI_GainSkill::Set_ActiveSkill(_int iTextureNum, std::function<void()> callback)
{
	m_iSkillNum = iTextureNum;

	switch (m_iSkillNum)
	{
	case 0:
		m_pUISkillTexture->Change_SrcColor({ 0.8f, 0.f, 1.f, 1.f });
		break;
	case 1:
		m_pUISkillTexture->Change_SrcColor({ 0.f, 0.f, 1.f, 1.f });
		break;
	case 2:
		break;
	}
	FSkillData SkillData = CFileData_Locator<FSkillData>::Get_FileData()->Get_Data(0, iTextureNum);

	m_pUISkillTexture->Change_Texture(L"Prototype_Component_Texture_Skill", iTextureNum);
	m_pUISkillTexture->Set_Text(SkillData.szSkillName);
	lstrcpy(m_szText, SkillData.szSkillDescription);

	m_bGainActive = true;
	m_bIsRewarded = false;

	m_Callback = callback;
}

void CUI_GainSkill::Set_SkillText()
{
	m_vecText.clear();

	_int len = (_int)wcslen(m_szText);
	_int index = 0;

	while (index < len)
	{
		size_t chunkSize = min(25, len - index);
		m_vecText.push_back(_wstring(m_szText + index, chunkSize));
		index += (_int)chunkSize;
	}
}

HRESULT CUI_GainSkill::Ready_PartObject()
{
	/* Board */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.pOwner = this;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_Board.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Board", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.pOwner = this;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_Border.dat";
		StretchDesc.fStretchFactor = 15.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_LeftBoard.dat";
		StretchDesc.UIDesc.bStretchY = true;
		StretchDesc.fStretchFactor = 50.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_LeftBoard", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_RightBoard.dat";
		StretchDesc.UIDesc.bStretchY = true;
		StretchDesc.fStretchFactor = 50.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_RightBoard", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_TopBoard.dat";
		StretchDesc.UIDesc.bStretchY = false;
		StretchDesc.fStretchFactor = 50.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_TopBoard", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	{
		StretchDesc.UIDesc.pOwner = this;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_BottomBoard.dat";
		StretchDesc.UIDesc.bStretchY = false;
		StretchDesc.fStretchFactor = 50.f;
		StretchDesc.bMiddle = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_BottomBoard", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	/* Icon */
	{
		TextureDesc.UIDesc.pOwner = this;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_Icon.dat";
		TextureDesc.UIDesc.eSorting = SORTING::MIDDLE;
	}
	if (FAILED(Add_PartObject(L"Part_UI_SkillIcon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUISkillTexture = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_SkillIcon" }));

	CUI_FixStretch::DESC FixStretchDesc{};
	{
		FixStretchDesc.UIDesc.pOwner = this;
		FixStretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_ButtonBorder.dat";
		FixStretchDesc.bMiddle = true;

		FixStretchDesc.fStartFactor = 10.f;
		FixStretchDesc.fEndFactor = 10.f;

		FixStretchDesc.pCur = &m_fButtonCur;
		FixStretchDesc.pMax = &m_fButtonMax;
	}
	if (FAILED(Add_PartObject(L"Part_UI_SkillButton", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretch", &FixStretchDesc)))
		return E_FAIL;

	/* E */
	{
		TextureDesc.UIDesc.pOwner = this;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Skill/UI_Skill_E.dat";
		TextureDesc.UIDesc.eSorting = SORTING::NONE;
		lstrcpy(TextureDesc.UIDesc.szText, L"Áö½Ä ½Àµæ");
	}
	if (FAILED(Add_PartObject(L"Part_UI_E", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_GainSkill* CUI_GainSkill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_GainSkill* pInstance = new CUI_GainSkill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_GainSkill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_GainSkill::Clone(void* pArg)
{
	CUI_GainSkill* pInstance = new CUI_GainSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_GainSkill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_GainSkill::Free()
{
    CUIObject::Free();
}

