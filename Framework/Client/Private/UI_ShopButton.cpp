#include "pch.h"
#include "UI_ShopButton.h"

#include "GameInstance.h"
#include "Item.h"
#include "UI_DynamicTexture.h"

CUI_ShopButton::CUI_ShopButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ShopButton::CUI_ShopButton(const CUI_ShopButton& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_ShopButton::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ShopButton::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_vLocate = { pDesc->vLocate.x, pDesc->vLocate.y, m_vLocate.z };
	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));
	
	m_fStretchFactor = pDesc->fStretchFactor;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	if (m_bStretchY)
		m_vScale.y *= m_fRatio;

	else
		m_vScale.x *= m_fRatio;

	m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, 1.f);

	return S_OK;
}

void CUI_ShopButton::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_ShopButton::Update(_float fTimeDelta)
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
				m_pGameInstance->Play_Sound(L"Button_Hover.wav", (_uint)SOUND_CHANNEL::UI, 0.9f);
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
			if (m_tParams.fThreshold > 0)
				m_tParams.fThreshold -= 0.1f;

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

	super::Update(fTimeDelta);
}

void CUI_ShopButton::Late_Update(_float fTimeDelta)
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

HRESULT CUI_ShopButton::Render()
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

	_float2 vTextPos = { m_vLocate.x - 195.f, m_vLocate.y + 7.f };
	m_pGameInstance->Render_Font(L"Font_Cafe_10", m_szItemCountText, vTextPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	vTextPos = { m_vLocate.x - 150.f, m_vLocate.y - 2.f };
	m_pGameInstance->Render_Font(L"Font_Cafe_15", m_szItemName, vTextPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	vTextPos = { m_vLocate.x + 190.f, m_vLocate.y + 5.f };
	m_pGameInstance->Render_Font(L"Font_Cafe_15", m_szItemPrice, vTextPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::RIGHT);

	return S_OK;
}

void CUI_ShopButton::Change_ItemTexture(_wstring strTextureTag, _int iTextureNum)
{
	m_pUIItemTexture->Change_Texture(strTextureTag, iTextureNum);
}

void CUI_ShopButton::Set_ItemData(_int iItemCount, const _tchar* szItemName, _int iItemPrice)
{
	_wstring strCount = L"x" + Format_With_Comma((_float)iItemCount);

	lstrcpy(m_szItemCountText, strCount.c_str());
	lstrcpy(m_szItemName, szItemName);
	lstrcpy(m_szItemPrice, Format_With_Comma((_float)iItemPrice).c_str());
}

HRESULT CUI_ShopButton::Ready_PartObject()
{
	/* Item */
	CUI_DynamicTexture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Button_Item.dat";
		TextureDesc.vLocate = {m_vLocate.x - 210.f, m_vLocate.y};
	}
	if (FAILED(Add_PartObject(L"Part_UI_Item", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &TextureDesc)))
		return E_FAIL;

	m_pUIItemTexture = static_cast<CUI_DynamicTexture*>(Find_PartObject({ L"Part_UI_Item" }));

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Shop/UI_Shop_Button_Micro.dat";
		TextureDesc.vLocate = { m_vLocate.x + 210.f, m_vLocate.y };
	}
	if (FAILED(Add_PartObject(L"Part_UI_Micro", LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &TextureDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_ShopButton* CUI_ShopButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ShopButton* pInstance = new CUI_ShopButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_ShopButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_ShopButton::Clone(void* pArg)
{
	CUI_ShopButton* pInstance = new CUI_ShopButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_ShopButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ShopButton::Free()
{
	CUI::Free();
}