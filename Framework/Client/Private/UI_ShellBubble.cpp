#include "pch.h"
#include "UI_ShellBubble.h"

#include "GameInstance.h"
#include "UI_Texture.h"

CUI_ShellBubble::CUI_ShellBubble(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ShellBubble::CUI_ShellBubble(const CUI_ShellBubble& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_ShellBubble::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ShellBubble::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	lstrcpy(m_szText, pDesc->szText);

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CUI_ShellBubble::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_ShellBubble::Update(_float fTimeDelta)
{
	m_pUICtrl->Set_Text(m_szText);

	if (lstrlen(m_szText) <= 2)
		m_fStretchFactor = 1.f;
	else
		m_fStretchFactor = m_fChangeRatio * (lstrlen(m_szText) - 2);

	m_pUICtrl->Set_Locate({75 - (lstrlen(m_szText) - 2) * 7.f, m_vCtrlLocate.y, m_vCtrlLocate.z});

	super::Update(fTimeDelta);
}

void CUI_ShellBubble::Late_Update(_float fTimeDelta)
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

HRESULT CUI_ShellBubble::Render()
{
	if (!m_pTextureCom)
		return S_OK;

	_float fRatio = (m_tParams.vStretchRange.y - m_tParams.vStretchRange.x) * (m_fStretchFactor - 1) + 1;

	m_pTransformCom->Set_Scale(m_vScale.x * fRatio, m_vScale.y, 1.f);

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", 0)))
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

HRESULT CUI_ShellBubble::Ready_PartObject()
{
	/* Ctrl */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.eSorting = SORTING::LEFT;
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_Ctrl.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Ctrl", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUICtrl = static_cast<CUI_Texture*>(Find_PartObject({L"Part_UI_Ctrl"}));
	m_vCtrlLocate = m_pUICtrl->Get_Locate();

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerLeft/UI_ShellSkillBoardTri.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Triangle", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_ShellBubble* CUI_ShellBubble::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ShellBubble* pInstance = new CUI_ShellBubble(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_ShellBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_ShellBubble::Clone(void* pArg)
{
	CUI_ShellBubble* pInstance = new CUI_ShellBubble(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_ShellBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ShellBubble::Free()
{
	CUI::Free();
}