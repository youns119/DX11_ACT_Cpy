#include "pch.h"
#include "UI_Shell.h"

#include "GameInstance.h"

#include "UI_Texture.h"
#include "UI_Umami.h"
#include "UI_ShellBubble.h"

CUI_Shell::CUI_Shell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Shell::CUI_Shell(const CUI_Shell& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Shell::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Shell::Initialize(void* pArg)
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

	return S_OK;
}

void CUI_Shell::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Shell::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (m_fFrame < 1.f)
		m_fFrame += 5.f * fTimeDelta;
	else
		m_fFrame = 1.f;

	super::Update(fTimeDelta);
}

void CUI_Shell::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

		if (m_fFrame >= 0.9f)
			super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Shell::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	m_pTransformCom->Set_Scale(m_vScale.x * fRatio * m_fFrame, m_vScale.y * m_fFrame, 1.f);

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStretchFactor", &fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

    return S_OK;
}

void CUI_Shell::Show()
{
	m_fFrame = 0;
	m_isActive = true;
}

void CUI_Shell::Hide()
{
}

void CUI_Shell::Set_Skill(_wstring strSkillName)
{
	m_pUIShellBubble->Set_Text(strSkillName.c_str());
}

void CUI_Shell::Set_ShellDef(_float fShellDef)
{
	_wstring strValue = Format_With_Comma(fShellDef);
	m_pUIShellDef->Set_Text(strValue.c_str());
}

HRESULT CUI_Shell::Ready_PartObject()
{
	/* ShellBubble */
	CUI_ShellBubble::DESC BubbleDesc{};
	{
		BubbleDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_ShellSkillBoard.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_ShellBubble", LEVEL_STATIC, L"Prototype_GameObject_UI_ShellBubble", &BubbleDesc)))
		return E_FAIL;

	m_pUIShellBubble = static_cast<CUI_ShellBubble*>(Find_PartObject({ L"Part_UI_ShellBubble" }));

	/* Shell_Def */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_Shell_Def.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Shell_Def", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUIShellDef = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Shell_Def" }));

	return S_OK;
}

CUI_Shell* CUI_Shell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Shell* pInstance = new CUI_Shell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Shell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Shell::Clone(void* pArg)
{
	CUI_Shell* pInstance = new CUI_Shell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Shell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Shell::Free()
{
    CUI::Free();
}

