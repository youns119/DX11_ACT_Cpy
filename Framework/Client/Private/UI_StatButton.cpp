#include "pch.h"
#include "UI_StatButton.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_Texture.h"
#include "UI_DynamicButton.h"

CUI_StatButton::CUI_StatButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_StatButton::CUI_StatButton(const CUI_StatButton& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_StatButton::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StatButton::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));
	
	m_fStretchFactor = pDesc->fStretchFactor;

	if (FAILED(Ready_PartObject(pDesc)))
		return E_FAIL;

	m_fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	if (m_bStretchY)
		m_vScale.y *= m_fRatio;

	else
		m_vScale.x *= m_fRatio;

	m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, 1.f);

	m_HoverCallback = pDesc->HoverCallback;
	m_pValue = pDesc->pValue;
	m_iValue = *m_pValue;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	m_pMicroplastic = pDesc->pMicroplastic;
	m_pLevelUpCost = pDesc->pLevelUpCost;
	m_pLevelDownCost = pDesc->pLevelDownCost;

	return S_OK;
}

void CUI_StatButton::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_StatButton::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	_float2 fMousePos = m_pGameInstance->Calulate_ScreenMousePos();

	_float2 vMin = { m_vLocate.x - m_vScale.x * 0.5f, m_vLocate.y - m_vScale.y * 0.5f };
	_float2 vMax = { m_vLocate.x + m_vScale.x * 0.5f, m_vLocate.y + m_vScale.y * 0.5f };

	m_bHover = (fMousePos.x >= vMin.x && fMousePos.x <= vMax.x && fMousePos.y >= vMin.y && fMousePos.y <= vMax.y);

	m_fFrame += fTimeDelta;

	if (m_bHover)
	{
		if (m_HoverCallback)
		{
			if (!m_bHovered)
			{
				m_HoverCallback();
				m_bHovered = true;
			}
		}

		if (m_pGameInstance->Mouse_Down(DIM_LB) && m_Callback)
			m_Callback();
	}
	else
		m_bHovered = false;

	switch (m_iShaderPass)
	{
	case (_uint)SHADER_PASS_UI::OPTION_BUTTON:
		if (m_fFrame >= m_fInterval)
		{
			if (m_tParams.fThreshold > 0.1f)
				m_tParams.fThreshold -= 0.1f;
			else
				m_tParams.fThreshold = 0.1f;

			m_tParams.vSpriteIndex.y++;

			if (m_tParams.vSpriteIndex.y >= m_tParams.vSpriteSize.y)
			{
				m_tParams.vSpriteIndex.y = 0;
				m_tParams.vSpriteIndex.x++;

				if (m_tParams.vSpriteIndex.x >= m_tParams.vSpriteSize.x)
					m_tParams.vSpriteIndex = {};
			}
			m_fFrame = 0.f;
		}
		break;
	default:
		if (m_fFrame >= m_fInterval)
		{
			m_tParams.vSpriteIndex.y++;

			if (m_tParams.vSpriteIndex.y >= m_tParams.vSpriteSize.y)
			{
				m_tParams.vSpriteIndex.y = 0;
				m_tParams.vSpriteIndex.x++;

				if (m_tParams.vSpriteIndex.x >= m_tParams.vSpriteSize.x)
					m_tParams.vSpriteIndex = {};
			}

			m_fFrame = 0.f;
		}
		break;
	}

	Set_UpButton();
	Set_DownButton();

	super::Update(fTimeDelta);
}

void CUI_StatButton::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
		
		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_StatButton::Render()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIParams", &m_tParams, sizeof(UI_PARAMS))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHover, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float2 vTextPos = { m_vLocate.x + 95.f, m_vLocate.y};
	_wstring strValue = to_wstring(m_iValue);

	m_pGameInstance->Render_Font(L"Font_Main_40", strValue.c_str(), vTextPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, {0.f, 0.f}, 0.5f, SORTING::MIDDLE);

	return S_OK;
}

void CUI_StatButton::Reset_Value()
{
	if (m_pValue)
		m_iValue = *m_pValue;
}

HRESULT CUI_StatButton::Ready_PartObject(DESC* pDesc)
{
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = pDesc->strIconFileName;
		lstrcpy(TextureDesc.UIDesc.szText, pDesc->szIconText);
	}
	if (FAILED(Add_PartObject(L"Part_UI_Icon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	CUI_DynamicButton::DESC ButtonDesc{};
	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_LeftButton.dat";
		ButtonDesc.vLocate = { m_vLocate.x + 60.f, m_vLocate.y };
	}
	if (FAILED(Add_PartObject(L"Part_UI_DownButton", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
		return E_FAIL;

	m_pDownButton = static_cast<CUI_DynamicButton*>(Find_PartObject({ L"Part_UI_DownButton" }));

	{
		ButtonDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Stat/UI_Stat_RightButton.dat";
		ButtonDesc.vLocate = { m_vLocate.x + 130.f, m_vLocate.y };
	}
	if (FAILED(Add_PartObject(L"Part_UI_UpButton", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicButton", &ButtonDesc)))
		return E_FAIL;

	m_pUpButton = static_cast<CUI_DynamicButton*>(Find_PartObject({ L"Part_UI_UpButton" }));

	return S_OK;
}

void CUI_StatButton::Set_UpButton()
{
	// 다음 레벨 비용!
	if (*m_pMicroplastic < *m_pLevelUpCost)
	{
		m_pUpButton->Change_SrcColor({ 0.5f, 0.5f, 0.5f, 1.f });
		m_pUpButton->Change_DstColor({ 0.5f, 0.5f, 0.5f, 1.f });
		m_pUpButton->Set_Callback([this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
			});
	}
	else
	{
		m_pUpButton->Change_SrcColor({ 0.f, 0.f, 0.f, 1.f });
		m_pUpButton->Change_DstColor({ 34.f / 255.f, 0.f, 197.f / 255.f, 1.f });
		m_pUpButton->Set_Callback([this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				*m_pMicroplastic -= *m_pLevelUpCost;
				m_iValue++;
			});
	}
}

void CUI_StatButton::Set_DownButton()
{
	if (m_iValue <= *m_pValue)
	{
		m_pDownButton->Change_SrcColor({ 0.5f, 0.5f, 0.5f, 1.f });
		m_pDownButton->Change_DstColor({ 0.5f, 0.5f, 0.5f, 1.f });
		m_pDownButton->Set_Callback([this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
			});
	}
	else
	{
		m_pDownButton->Change_SrcColor({ 0.f, 0.f, 0.f, 1.f });
		m_pDownButton->Change_DstColor({ 34.f / 255.f, 0.f, 197.f / 255.f, 1.f });
		m_pDownButton->Set_Callback([this]()->void
			{
				m_pGameInstance->Play_Sound(L"InventoryClickNext.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
				m_iValue--;
				*m_pMicroplastic += *m_pLevelDownCost;
			});
	}
}

CUI_StatButton* CUI_StatButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StatButton* pInstance = new CUI_StatButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_StatButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_StatButton::Clone(void* pArg)
{
	CUI_StatButton* pInstance = new CUI_StatButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_StatButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StatButton::Free()
{
	CUI::Free();
}