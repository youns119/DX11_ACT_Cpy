#include "pch.h"
#include "UI_Peril.h"

#include "GameInstance.h"

CUI_Peril::CUI_Peril(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Peril::CUI_Peril(const CUI_Peril& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Peril::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Peril::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_pIsGrab = pDesc->pIsGrab;
	m_pIsUnblockable = pDesc->pIsUnblockable;


	m_vRedColor = m_tParams.vSrcColor;
	m_vBlueColor = m_tParams.vDstColor;

	return S_OK;
}

void CUI_Peril::Priority_Update(_float fTimeDelta)
{
}

void CUI_Peril::Update(_float fTimeDelta)
{
	if (m_pIsGrab && *m_pIsGrab)
	{
		if (!m_isActive && m_vRedColor.w > 0.0f)
		{
			m_isActive = true;
			// 사운드
			m_pGameInstance->Play_Sound(L"Peril.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);
		}
		
		m_iTextureNum = 0;

		CGameObject* pPlayer = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").back();
		Set_ObjectLocate(pPlayer, { 0.f, -150.f });
		m_fFrame += fTimeDelta * 0.5f;
		_float fScale = min(4.f, 1.f + m_fFrame);
		m_pTransformCom->Set_Scale(m_vScale.x * fScale, m_vScale.y * fScale, 1.f);

		m_vRedColor.w -= m_fFrame;

		Change_SrcColor(m_vRedColor);
		m_iTextureNum = 0;

		if (m_vRedColor.w <= 0.0f)
		{
			m_isActive = false;
		}
	}
	else if (m_pIsUnblockable && *m_pIsUnblockable)
	{
		if (!m_isActive && m_vBlueColor.w > 0.0f)
		{
			m_isActive = true;
			// 사운드
			m_pGameInstance->Play_Sound(L"Peril.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);
		}
		m_iTextureNum = 1;

		CGameObject* pPlayer = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").back();
		Set_ObjectLocate(pPlayer, { 0.f, -150.f });

		m_fFrame += fTimeDelta * 0.5f;
		_float fScale = min(4.f, 1.f + m_fFrame);

		m_pTransformCom->Set_Scale(m_vScale.x * fScale, m_vScale.y * fScale, 1.f);

		m_vBlueColor.w -= m_fFrame;

		Change_SrcColor(m_vBlueColor);
		m_iTextureNum = 1;

		if (m_vBlueColor.w <= 0.0f)
		{
			m_isActive = false;
		}
	}
	else
	{
		m_isActive = false;
		m_fFrame = 0.f;
		m_vRedColor.w = 1.f;
		m_vBlueColor.w = 1.f;
	}
}

void CUI_Peril::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_Peril::Render()
{
	if (!m_pTextureCom)
		return S_OK;

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

	return S_OK;
}

CUI_Peril* CUI_Peril::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Peril* pInstance = new CUI_Peril(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Peril");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Peril::Clone(void* pArg)
{
	CUI_Peril* pInstance = new CUI_Peril(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Peril");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Peril::Free()
{
	CUI::Free();
}