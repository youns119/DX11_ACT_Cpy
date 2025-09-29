#include "pch.h"
#include "UI_Option.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_DefaultStretch.h"
#include "UI_DynamicTexture.h"
#include "UI_DynamicButton.h"

CUI_Option::CUI_Option(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Option::CUI_Option(const CUI_Option& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Option::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Option::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;
	pDesc->UIObjectDesc.fZ = 0.01f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	m_pOwner = pDesc->pOwner;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	m_isActive = false;

	CRenderer::RENDER_OPTION RenderOption = m_pGameInstance->Get_RenderOption();
	m_bOn[0] = !RenderOption.isSkipSSAO;
	m_bOn[2] = !RenderOption.isSkipBloom;
	m_bOn[3] = !RenderOption.isHardShadow;

	// 영상 촬영용 BGM 제거
	m_iVolum[1] = 0;
	m_pGameInstance->Set_ChannelVolume((_uint)SOUND_CHANNEL::BGM, 0.f);
	//

	return S_OK;
}

void CUI_Option::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_pUIManager->Get_CopyTexture_IsActive())
		m_eRenderGroup = CRenderer::RENDERGROUP::UI_ANIM;
	else
		m_eRenderGroup = CRenderer::RENDERGROUP::UI;

	super::Priority_Update(fTimeDelta);
}

void CUI_Option::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	for (_int i = 0; i < 5; i++)
	{
		_float3 vLocate = m_pUIDot[i]->Get_Locate();

		if (m_bOn[i])
		{
			if (vLocate.x < 620.f)
			{
				vLocate.x += fTimeDelta * 300.f;
				m_pUIBar[i]->Change_SrcColor({ 0.f, 0.f, 0.f, 1.f });
			}
			else
			{
				vLocate.x = 620.f;
				m_pUIBar[i]->Change_SrcColor({ 5.f / 255.f, 175.f / 255.f, 242.f / 255.f, 1.f });
			}
			m_pUIDot[i]->Set_Locate(vLocate);
		}
		else
		{
			if (vLocate.x > 570.f)
			{
				vLocate.x -= fTimeDelta * 300.f;
				m_pUIBar[i]->Change_SrcColor({ 5.f / 255.f, 175.f / 255.f, 242.f / 255.f, 1.f });
			}
			else
			{
				vLocate.x = 570.f;
				m_pUIBar[i]->Change_SrcColor({ 0.f, 0.f, 0.f, 1.f });
			}
			m_pUIDot[i]->Set_Locate(vLocate);
		}

		m_iVolum[i] = (m_pVolumeBar[i]->Get_Locate().x - 860.f) / 150.f * 100.f;
	}


	for (_uint i = 0; i < SOUND_END; i++)
	{
		if (i == (_uint)SOUND_CHANNEL::BGM)
			m_pGameInstance->Set_ChannelVolume(i, m_iVolum[1] / 100.f);
		else if (i == (_uint)SOUND_CHANNEL::AMBIENCE || i == (_uint)SOUND_CHANNEL::PLAYER_AMBIENCE)
			m_pGameInstance->Set_ChannelVolume(i, m_iVolum[3] / 100.f);
		else if (i == (_uint)SOUND_CHANNEL::CONVERSATION1 || i == (_uint)SOUND_CHANNEL::CONVERSATION2 || i == (_uint)SOUND_CHANNEL::PLAYER_VOICE)
			m_pGameInstance->Set_ChannelVolume(i, m_iVolum[4] / 100.f);
		else
			m_pGameInstance->Set_ChannelVolume(i, m_iVolum[2] / 100.f);
	}

	m_pGameInstance->Set_MasterVolume(m_iVolum[0] /100.f);

	super::Update(fTimeDelta);
}

void CUI_Option::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_Option::Render()
{
	m_pGameInstance->Render_Font(L"Font_Main_40", L"그래픽", { 240.f, 177.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.7f);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"그림자", { 240.f, 397.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.7f);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"오디오", { 700.f, 177.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 0.7f);

	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"마스터 볼륨", { 705.f, 237.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"음악 볼륨", { 730.f, 292.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"효과음 볼륨", { 730.f, 347.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"환경 볼륨", { 730.f, 402.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Font(L"Font_Cafe_15", L"음성 볼륨", { 730.f, 457.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pGameInstance->Render_Font(L"Font_Cafe_15", (to_wstring(m_iVolum[0]) + L"%").c_str(), {1080.f, 249.f}, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, {0.f, 0.f}, 1.f, SORTING::RIGHT);
	m_pGameInstance->Render_Font(L"Font_Cafe_15", (to_wstring(m_iVolum[1]) + L"%").c_str(), { 1080.f, 304.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);
	m_pGameInstance->Render_Font(L"Font_Cafe_15", (to_wstring(m_iVolum[2]) + L"%").c_str(), { 1080.f, 359.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);
	m_pGameInstance->Render_Font(L"Font_Cafe_15", (to_wstring(m_iVolum[3]) + L"%").c_str(), { 1080.f, 414.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);
	m_pGameInstance->Render_Font(L"Font_Cafe_15", (to_wstring(m_iVolum[4]) + L"%").c_str(), { 1080.f, 469.f }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);

    return S_OK;
}

HRESULT CUI_Option::Ready_PartObject()
{
	CUI_DefaultStretch::DESC StretchDesc{};
	{
		StretchDesc.UIDesc.pOwner = m_pOwner;
		StretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_Border.dat";
		StretchDesc.fStretchFactor = 28.f;
		StretchDesc.bMiddle = true;
		StretchDesc.UIDesc.bRightAngle = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_DefaultStretch", &StretchDesc)))
		return E_FAIL;

	CUI_DynamicButton::DESC ButtonDesc{};
	ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_Button.dat";
	ButtonDesc.UIDesc.pOwner = m_pOwner;
	ButtonDesc.HoverCallback = [this]()->void
		{
			m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.3f);
		};

	CUI_DynamicTexture::DESC BarDesc{};
	CUI_DynamicTexture::DESC DotDesc{};

	BarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_Bar.dat";
	BarDesc.UIDesc.pOwner = m_pOwner;

	DotDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_Dot.dat";
	DotDesc.UIDesc.pOwner = m_pOwner;



	{
		ButtonDesc.vLocate = { 440.f, 252.f };
		lstrcpy(ButtonDesc.szText, L"앰비언트 오클루젼");
		ButtonDesc.Callback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				m_bOn[0] = !m_bOn[0];
				CRenderer::RENDER_OPTION RenderOption = m_pGameInstance->Get_RenderOption();
				RenderOption.isSkipSSAO = !m_bOn[0];
				m_pGameInstance->Set_RenderOption(RenderOption);
			};

		BarDesc.vLocate = { 595.f, 252.f };
		DotDesc.vLocate = { 620.f, 252.f };
	
		if (FAILED(Add_PartObject(L"Part_UI_Button1", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Bar1", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &BarDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Dot1", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &DotDesc)))
			return E_FAIL;
	
		m_pUIBar[0] = static_cast<CUI_DynamicTexture*>(Find_PartObject({L"Part_UI_Bar1"}));
		m_pUIDot[0] = static_cast<CUI_DynamicTexture*>(Find_PartObject({L"Part_UI_Dot1"}));
	}

	{
		ButtonDesc.vLocate = { 440.f, 302.f };
		lstrcpy(ButtonDesc.szText, L"선 섀프트");
		ButtonDesc.Callback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				m_bOn[1] = !m_bOn[1];
			};

		BarDesc.vLocate = { 595.f, 302.f };
		DotDesc.vLocate = { 620.f, 302.f };
	
		if (FAILED(Add_PartObject(L"Part_UI_Button2", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Bar2", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &BarDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Dot2", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &DotDesc)))
			return E_FAIL;

		m_pUIBar[1] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Bar2" }));
		m_pUIDot[1] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Dot2" }));
	}
	

	{
		ButtonDesc.vLocate = { 440.f, 352.f };
		lstrcpy(ButtonDesc.szText, L"블룸");
		ButtonDesc.Callback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				m_bOn[2] = !m_bOn[2];
				CRenderer::RENDER_OPTION RenderOption = m_pGameInstance->Get_RenderOption();
				RenderOption.isSkipBloom = !m_bOn[2];
				m_pGameInstance->Set_RenderOption(RenderOption);
			};

		BarDesc.vLocate = { 595.f, 352.f };
		DotDesc.vLocate = { 620.f, 352.f };

		if (FAILED(Add_PartObject(L"Part_UI_Button3", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Bar3", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &BarDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Dot3", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &DotDesc)))
			return E_FAIL;

		m_pUIBar[2] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Bar3" }));
		m_pUIDot[2] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Dot3" }));
	}

	{
		ButtonDesc.vLocate = { 440.f, 472.f };
		lstrcpy(ButtonDesc.szText, L"높은 그림자 품질");
		ButtonDesc.Callback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				m_bOn[3] = !m_bOn[3];
				CRenderer::RENDER_OPTION RenderOption = m_pGameInstance->Get_RenderOption();
				RenderOption.isHardShadow = !m_bOn[3];
				m_pGameInstance->Set_RenderOption(RenderOption);
			};

		BarDesc.vLocate = { 595.f, 472.f };
		DotDesc.vLocate = { 620.f, 472.f };

		if (FAILED(Add_PartObject(L"Part_UI_Button4", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Bar4", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &BarDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Dot4", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &DotDesc)))
			return E_FAIL;

		m_pUIBar[3] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Bar4" }));
		m_pUIDot[3] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Dot4" }));
	}

	{
		ButtonDesc.vLocate = { 440.f, 522.f };
		lstrcpy(ButtonDesc.szText, L"물 그림자");
		ButtonDesc.Callback = [this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				m_bOn[4] = !m_bOn[4];

				CGameObject* pWaterShadow = 
					m_pGameInstance->Find_GameObject_With_Name(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Environment", L"WaterShadow");

				if (nullptr != pWaterShadow)
					pWaterShadow->Set_IsActive(m_bOn[4]);
			};

		BarDesc.vLocate = { 595.f, 522.f };
		DotDesc.vLocate = { 620.f, 522.f };

		if (FAILED(Add_PartObject(L"Part_UI_Button5", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Bar5", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &BarDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(L"Part_UI_Dot5", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &DotDesc)))
			return E_FAIL;

		m_pUIBar[4] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Bar5" }));
		m_pUIDot[4] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Dot5" }));
	}

	CUI_DynamicTexture::DESC LeftLineDesc{};
	LeftLineDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_LeftLine.dat";
	LeftLineDesc.UIDesc.pOwner = m_pOwner;

	CUI_DynamicTexture::DESC LineDesc{};
	LineDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_Line.dat";
	LineDesc.UIDesc.pOwner = m_pOwner;

	CUI_DynamicTexture::DESC RightLineDesc{};
	RightLineDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_RightLine.dat";
	RightLineDesc.UIDesc.pOwner = m_pOwner;
	
	CUI_DynamicTexture::DESC RectDesc{};
	RectDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_Rect.dat";
	RectDesc.UIDesc.pOwner = m_pOwner;

	CUI_DynamicButton::DESC LineButtonDesc{};
	LineButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Menu/Option/UI_LineButton.dat";
	LineButtonDesc.UIDesc.pOwner = m_pOwner;

	LineButtonDesc.Callback = [this]()->void
		{
		_float2 vMousePos = m_pGameInstance->Calulate_ScreenMousePos();

		_float3 vLocate = m_pVolumeBar[0]->Get_Locate();
		vLocate.x = vMousePos.x;

		if (vLocate.x < 860.f)
			vLocate.x = 860.f;

		if (vLocate.x > 1010.f)
			vLocate.x = 1010.f;

		m_pVolumeBar[0]->Set_Locate(vLocate);
		};

	LeftLineDesc.vLocate = { 860.f, 247.f };
	LineDesc.vLocate = { 935.f, 247.f };
	RightLineDesc.vLocate = { 1010.f, 247.f };
	RectDesc.vLocate = { 1010.f, 247.f };
	LineButtonDesc.vLocate = { 935.f, 247.f };

	if (FAILED(Add_PartObject(L"Part_UI_LeftLine", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &LeftLineDesc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(L"Part_UI_Line", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &LineDesc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(L"Part_UI_RightLine", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &RightLineDesc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(L"Part_UI_Rect", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &RectDesc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(L"Part_UI_LineButton", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &LineButtonDesc)))
		return E_FAIL;

	m_pVolumeBar[0] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Rect" }));

	_float fStartY = 247.f;
	_float fOffsetY = 55.f;

	for (_int i = 1; i < 5; i++)
	{
		_wstring strLeftLine = L"Part_UI_LeftLine" + to_wstring(i);
		_wstring strLine = L"Part_UI_Line" + to_wstring(i);
		_wstring strRightLine = L"Part_UI_RightLine" + to_wstring(i);
		_wstring strRect = L"Part_UI_Rect" + to_wstring(i);
		_wstring strLineButton = L"Part_UI_LineButton" + to_wstring(i);

		LeftLineDesc.vLocate = { 860.f, fStartY + fOffsetY * i };
		LineDesc.vLocate = { 935.f, fStartY + fOffsetY * i };
		RightLineDesc.vLocate = { 1010.f, fStartY + fOffsetY * i };
		RectDesc.vLocate = { 1010.f, fStartY + fOffsetY * i };
		LineButtonDesc.vLocate = { 935.f, fStartY + fOffsetY * i };

		LineButtonDesc.Callback = [this, i]()->void
		{
			_float2 vMousePos = m_pGameInstance->Calulate_ScreenMousePos();

			_float3 vLocate = m_pVolumeBar[i]->Get_Locate();
			vLocate.x = vMousePos.x;

			if (vLocate.x < 860.f)
				vLocate.x = 860.f;

			if (vLocate.x > 1010.f)
				vLocate.x = 1010.f;

			m_pVolumeBar[i]->Set_Locate(vLocate);
		};

		if (FAILED(Add_PartObject(strLeftLine, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &LeftLineDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(strLine, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &LineDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(strRightLine, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &RightLineDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(strRect, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &RectDesc)))
			return E_FAIL;
		if (FAILED(Add_PartObject(strLineButton, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &LineButtonDesc)))
			return E_FAIL;

		m_pVolumeBar[i] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ strRect }));
	}

	return S_OK;
}

CUI_Option* CUI_Option::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Option* pInstance = new CUI_Option(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Option");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Option::Clone(void* pArg)
{
	CUI_Option* pInstance = new CUI_Option(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Option");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Option::Free()
{
    CUIObject::Free();
}

