#include "Camera.h"

#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& Prototype)
    : super(Prototype)
{
}

HRESULT CCamera::Initialize_Prototype()
{
    super::Initialize_Prototype();

    return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    m_pTransformCom->Locate(pDesc->vEye);
    m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));

    m_fFovY = pDesc->fFovY;
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;

    _uint2 vViewportSize = m_pGameInstance->Get_ViewportSize();
    m_fAspect = vViewportSize.x / static_cast<_float>(vViewportSize.y);

    return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
    return S_OK;
}

_bool CCamera::IsCurrCam() const
{
    return this == m_pGameInstance->Get_CurrCam();
}

void CCamera::Set_Transform_To_PipeLine()
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->
        Set_Transform(CPipeLine::TRANSFORM::VIEW, m_pTransformCom->Get_WorldMatrix_Inversed());
    m_pGameInstance->
        Set_Transform(CPipeLine::TRANSFORM::PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Free()
{
    super::Free();
}