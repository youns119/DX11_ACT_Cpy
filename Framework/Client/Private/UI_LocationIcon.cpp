#include "pch.h"
#include "UI_LocationIcon.h"

#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"

CUI_LocationIcon::CUI_LocationIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_LocationIcon::CUI_LocationIcon(const CUI_LocationIcon& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_LocationIcon::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_LocationIcon::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pNPCWorld = pDesc->pNPCWorld;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	return S_OK;
}

void CUI_LocationIcon::Priority_Update(_float fTimeDelta)
{
	_mat vObjectWorld = XMLoadFloat4x4(m_pNPCWorld);
	_vec4 vObjectPos = vObjectWorld.position;

	_mat matViewInv = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM::VIEW);
	_vec4 vCamPos = matViewInv.position;

	_float fDistance = (vObjectPos - vCamPos).Length();
	fDistance = min(fDistance, 35.f);

	_vec4 vToObject = (vObjectPos - vCamPos).Normalized();

	_vector vCamLook = matViewInv.look._vector();
	vCamLook = XMVector3Normalize(XMVectorSetY(vCamLook, 0.f)); // Y 성분 제거

	_float fAngle = atan2f(XMVectorGetZ(vToObject._vector()), XMVectorGetX(vToObject._vector())) -
		atan2f(XMVectorGetZ(vCamLook), XMVectorGetX(vCamLook));

	_vec4 vUIPos;
	vUIPos.x = sinf(fAngle) * fDistance;
	vUIPos.y = cosf(fAngle) * fDistance;

	m_pTransformCom->Locate(m_vLocate.x - vUIPos.x - s_fViewportWidth * 0.5f,
		-(m_vLocate.y - vUIPos.y) + s_fViewportHeight * 0.5f,
		m_vLocate.z);

	super::Priority_Update(fTimeDelta);
}

void CUI_LocationIcon::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CUI_LocationIcon::Late_Update(_float fTimeDelta)
{
	if (m_pUIManager->Get_DialogueUI_IsActive() || m_pUIManager->Get_CutSceneUI_IsActive())
		return;

	m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CUI_LocationIcon::Render()
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

    return S_OK;
}

CUI_LocationIcon* CUI_LocationIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LocationIcon* pInstance = new CUI_LocationIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_LocationIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_LocationIcon::Clone(void* pArg)
{
	CUI_LocationIcon* pInstance = new CUI_LocationIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_LocationIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LocationIcon::Free()
{
    CUI::Free();
}