#include "pch.h"
#include "UI_Notice.h"

#include "GameInstance.h"
#include "FileData_Locator.h"
#include "FData.h"
#include "UI_Texture.h"

CUI_Notice::CUI_Notice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Notice::CUI_Notice(const CUI_Notice& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Notice::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Notice::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_isActive = false;

	m_vItemLocate = m_pUIGainItem->Get_Locate();

	return S_OK;
}

void CUI_Notice::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Notice::Update(_float fTimeDelta)
{
	m_fFrame += fTimeDelta;

	if (m_fFrame < 0.5f)
	{
		m_fOffsetY = m_fFrame * 150.f;
	}
	else if (m_fFrame < 2.f)
	{
	}
	else if (m_fFrame < 2.5f)
	{
		m_fOffsetY = (m_fFrame - 1.5f) * 150.f;
	}
	else if (m_fFrame >= 2.5f)
	{
		m_isActive = false;
		m_fOffsetY = 0.f;
	}

	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f + m_fOffsetY, m_vLocate.z);
	m_pUIGainItem->Set_Locate({ m_vItemLocate.x, m_vItemLocate.y - m_fOffsetY, m_vItemLocate.z});

	super::Update(fTimeDelta);
}

void CUI_Notice::Late_Update(_float fTimeDelta)
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

HRESULT CUI_Notice::Render()
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

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y - m_fOffsetY };

	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, { 0.f, 0.f }, m_fTextScale, SORTING::MIDDLE);

	return S_OK;
}

void CUI_Notice::Show(CItem_Trash* pItem)
{
	_int iItemNum = pItem->Get_ItemNum();

	FItemData ItemData = CFileData_Locator<FItemData>::Get_FileData()->Get_Data(0, iItemNum);

	m_pUIGainItem->Change_Texture(pItem->Get_TextureTag(), iItemNum);
	lstrcpy(m_szText, ItemData.szName);
	m_fFrame = 0.f;
	m_isActive = true;
}

HRESULT CUI_Notice::Ready_PartObject()
{
	/* GainItem */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_GainItem.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_GainItem", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIGainItem = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_GainItem" }));

	return S_OK;
}

CUI_Notice* CUI_Notice::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Notice* pInstance = new CUI_Notice(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Notice");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Notice::Clone(void* pArg)
{
	CUI_Notice* pInstance = new CUI_Notice(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Notice");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Notice::Free()
{
	CUI::Free();
}