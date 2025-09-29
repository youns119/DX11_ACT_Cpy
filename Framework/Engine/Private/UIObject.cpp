#include "UIObject.h"

#include "GameInstance.h"

_float4x4 CUIObject::s_ViewMatrix {};
_float4x4 CUIObject::s_ProjMatrix {};

_float CUIObject::s_fViewportWidth { 0.f };
_float CUIObject::s_fViewportHeight { 0.f };
_float CUIObject::s_fWidthMiddle { 0.f };
_float CUIObject::s_fHeightMiddle { 0.f };


CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CUIObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	if (FAILED(CGameObject::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	////_uint2 vViewportSize = m_pGameInstance->Get_ViewportSize();
	//_uint iNumViewports{ 1 };
	//D3D11_VIEWPORT ViewportDesc{};
	//m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_pTransformCom->Set_Scale(pDesc->fSizeX, pDesc->fSizeY, 1.f);
	m_pTransformCom->Locate(pDesc->fX - s_fViewportWidth * 0.5f, -pDesc->fY + s_fViewportHeight * 0.5f, pDesc->fZ);

	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUIObject::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CUIObject::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

HRESULT CUIObject::Render()
{
	return S_OK;
}

void CUIObject::Resize(ID3D11DeviceContext* pContext)
{
	_uint iNumViewports { 1 };
	D3D11_VIEWPORT ViewportDesc {};
	pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	s_fViewportWidth = ViewportDesc.Width;
	s_fViewportHeight = ViewportDesc.Height;
	s_fWidthMiddle = ViewportDesc.Width * 0.5f;
	s_fHeightMiddle = ViewportDesc.Height * 0.5f;

	XMStoreFloat4x4(&s_ViewMatrix, XMMatrixIdentity()); //이미 했음
	XMStoreFloat4x4(&s_ProjMatrix, XMMatrixOrthographicLH(s_fViewportWidth, s_fViewportHeight, 0.f, 1.f));
}

void CUIObject::Free()
{
	CGameObject::Free();
}
