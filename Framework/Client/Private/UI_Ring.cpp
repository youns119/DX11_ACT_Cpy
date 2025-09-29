#include "pch.h"
#include "UI_Ring.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Ring::CUI_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Ring::CUI_Ring(const CUI_Ring& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Ring::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Ring::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_pInAir = pDesc->pInAir;
	m_pGrappling = pDesc->pGrappling;
	m_pSwimmingTimeAcc = pDesc->pSwimmingTimeAcc;
	m_pSwimmingMaxTime = pDesc->pSwimmingMaxTime;

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_isActive = false;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	return S_OK;
}

void CUI_Ring::Priority_Update(_float fTimeDelta)
{
}

void CUI_Ring::Update(_float fTimeDelta)
{
	if(*m_pSwimmingTimeAcc > 0.f)
		m_isActive = true;
	
	if (false == *m_pInAir || m_fRatio >= 1.f || true == *m_pGrappling)
		m_isActive = false;

	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, -m_vLocate.y + s_fViewportHeight * 0.5f - *m_pSwimmingTimeAcc * 50.f, m_vLocate.z);

	m_fRatio = *m_pSwimmingTimeAcc / *m_pSwimmingMaxTime;
}

void CUI_Ring::Late_Update(_float fTimeDelta)
{
	if (m_pUIManager->Get_IsActive_MenuUI())
		return;

	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Ring::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

    return S_OK;
}

CUI_Ring* CUI_Ring::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Ring* pInstance = new CUI_Ring(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Ring");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Ring::Clone(void* pArg)
{
	CUI_Ring* pInstance = new CUI_Ring(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Ring");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Ring::Free()
{
    CUI::Free();
}
