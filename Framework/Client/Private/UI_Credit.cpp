#include "pch.h"
#include "UI_Credit.h"

#include "GameInstance.h"

CUI_Credit::CUI_Credit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Credit::CUI_Credit(const CUI_Credit& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Credit::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Credit::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	return S_OK;
}

void CUI_Credit::Priority_Update(_float fTimeDelta)
{
}

void CUI_Credit::Update(_float fTimeDelta)
{
	if (m_isActive)
		m_fFrame += fTimeDelta;

	if (m_fFrame >= 20.f)
	{
		m_fFrame = 0.f;
		m_isActive = false;
	}
}

void CUI_Credit::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Credit::Render()
{
	if (!m_pTextureCom)
		return S_OK;

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
	
	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGameInstance->Render_Font(L"Font_Main_40", L"김태훈",				{ 640.f, 720.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {0.f, 0.f}, 1.f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"이펙트",				{ 640.f, 770.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {0.f, 0.f}, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"",					{ 640.f, 820.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {0.f, 0.f}, 1.f, SORTING::MIDDLE);
	
	m_pGameInstance->Render_Font(L"Font_Main_40", L"박상은",				{ 640.f, 870.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"UI",				{ 640.f, 920.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"아이템 & 인벤토리",	{ 640.f, 970.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"NPC & 상점",			{ 640.f, 1020.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"컷씬",				{ 640.f, 1070.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"미니게임",			{ 640.f, 1120.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"",					{ 640.f, 1170.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);

	m_pGameInstance->Render_Font(L"Font_Main_40", L"봉민지",				{ 640.f, 1220.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"맵",					{ 640.f, 1270.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"조명",				{ 640.f, 1320.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"",					{ 640.f, 1370.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);

	m_pGameInstance->Render_Font(L"Font_Main_40", L"안주헌",				{ 640.f, 1420.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"몬스터",				{ 640.f, 1470.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"",					{ 640.f, 1520.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);


	m_pGameInstance->Render_Font(L"Font_Main_40", L"유규빈",				{ 640.f, 1570.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {0.f, 0.f}, 1.f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"메인 프레임워크",		{ 640.f, 1620.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {0.f, 0.f}, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"메인 셰이더",			{ 640.f, 1670.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {0.f, 0.f}, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"PhysX",				{ 640.f, 1720.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {0.f, 0.f}, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"",					{ 640.f, 1770.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);

	m_pGameInstance->Render_Font(L"Font_Main_40", L"정연욱",				{ 640.f, 1820.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 1.f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"플레이어",			{ 640.f, 1870.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"애니메이션",			{ 640.f, 1920.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);
	m_pGameInstance->Render_Font(L"Font_Main_40", L"카메라",				{ 640.f, 1970.f - m_fFrame * m_fSpeed }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, { 0.f, 0.f }, 0.6f, SORTING::MIDDLE);

	return S_OK;
}

CUI_Credit* CUI_Credit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Credit* pInstance = new CUI_Credit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Credit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Credit::Clone(void* pArg)
{
	CUI_Credit* pInstance = new CUI_Credit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Credit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Credit::Free()
{
	CUI::Free();
}