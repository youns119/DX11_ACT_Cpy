#include "pch.h"
#include "UI_CutScene.h"

#include "GameInstance.h"

CUI_CutScene::CUI_CutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_CutScene::CUI_CutScene(const CUI_CutScene& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_CutScene::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CutScene::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;
	
	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_isActive = false;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));


	return S_OK;
}

void CUI_CutScene::Priority_Update(_float fTimeDelta)
{
}

void CUI_CutScene::Update(_float fTimeDelta)
{
}

void CUI_CutScene::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_CutScene::Render()
{
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fFrame, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_UI::LOCK)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGameInstance->Render_Font(L"Font_Cafe_15", m_szText, { 640.f, 665.f }, XMVectorSet(1.f, 1.f, 0.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);

	return S_OK;
}

CUI_CutScene* CUI_CutScene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CutScene* pInstance = new CUI_CutScene(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_CutScene::Clone(void* pArg)
{
	CUI_CutScene* pInstance = new CUI_CutScene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_CutScene::Free()
{
	CUI::Free();
}