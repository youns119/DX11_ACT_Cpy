#include "PixelPicking.h"

#include "GameInstance.h"

CPixelPicking::CPixelPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPixelPicking::Initialize()
{
    D3D11_TEXTURE2D_DESC TextureDesc {};
    _uint				iNumViewports = { 1 };
    D3D11_VIEWPORT		ViewportDesc {};

    ZeroMemory(&TextureDesc, sizeof(TextureDesc));
    ZeroMemory(&ViewportDesc, sizeof(ViewportDesc));

    //m_pGameInstance->Get_ViewportSize();

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    m_fViewportWidth = ViewportDesc.Width;
    m_fViewportHeight = ViewportDesc.Height;

    TextureDesc.Width = static_cast<_uint>(ViewportDesc.Width);
    TextureDesc.Height = static_cast<_uint>(ViewportDesc.Height);
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
        return E_FAIL;


    ZeroMemory(&TextureDesc, sizeof(TextureDesc));
    ZeroMemory(&ViewportDesc, sizeof(ViewportDesc));
    iNumViewports = 1;

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    m_fViewportWidth = ViewportDesc.Width;
    m_fViewportHeight = ViewportDesc.Height;

    TextureDesc.Width = static_cast<_uint>(ViewportDesc.Width);
    TextureDesc.Height = static_cast<_uint>(ViewportDesc.Height);
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTextureObjectID)))
        return E_FAIL;

    return S_OK;
}

void CPixelPicking::Update()
{
    m_pGameInstance->Copy_RT_Resource(TEXT("Target_Depth"), m_pTexture2D);
    m_pGameInstance->Copy_RT_Resource(TEXT("Target_ObjectID"), m_pTextureObjectID);
}

_bool CPixelPicking::Compute_PickPos(_float3& Out)
{
    if (false == m_pGameInstance->Check_IsAppFocused())
        return false;

    _float2 vMouse = m_pGameInstance->Calulate_ScreenMousePos();
    if (Is_OutOfBoundary(vMouse))
        return false;


    D3D11_MAPPED_SUBRESOURCE SubResource {};

    _uint iPixelIndex = static_cast<_uint>(vMouse.y * m_fViewportWidth + vMouse.x);

    /* 텍스체에 값을 채운다. 수정 변경한다. */
    m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);
    _float4 vDepthDesc = static_cast<_float4*>(SubResource.pData)[iPixelIndex];

    m_pContext->Unmap(m_pTexture2D, 0);

    if (0.f == vDepthDesc.z)
        return false;

    _vector			vWorldPos = XMVectorSet(vMouse.x, vMouse.y, 0.f, 0.f);

    vWorldPos = XMVectorSetX(vWorldPos, vMouse.x / (m_fViewportWidth * 0.5f) - 1.f);
    vWorldPos = XMVectorSetY(vWorldPos, vMouse.y / (m_fViewportHeight * -0.5f) + 1.f);
    vWorldPos = XMVectorSetZ(vWorldPos, vDepthDesc.x);
    vWorldPos = XMVectorSetW(vWorldPos, 1.f);

    vWorldPos = XMVector3TransformCoord(vWorldPos, m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::PROJ));
    vWorldPos = XMVector3TransformCoord(vWorldPos, m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW));

    XMStoreFloat3(&Out, vWorldPos);

    return TRUE;
}

_bool CPixelPicking::Compute_ObjectID(_uint& Out)
{
    if (false == m_pGameInstance->Check_IsAppFocused())
        return false;

    _float2 vMouse = m_pGameInstance->Calulate_ScreenMousePos();
    if (Is_OutOfBoundary(vMouse))
        return false;


    D3D11_MAPPED_SUBRESOURCE SubResource {};

    _uint iPixelIndex = static_cast<_uint>(vMouse.y * m_fViewportWidth + vMouse.x);

    m_pContext->Map(m_pTextureObjectID, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);
    _uint iID = static_cast<_uint*>(SubResource.pData)[iPixelIndex];

    m_pContext->Unmap(m_pTextureObjectID, 0);

    Out = iID;
    return (0 != iID);
}

_vector CPixelPicking::Compute_WorldPos(const _float2& ViewportPos)
{
    D3D11_MAPPED_SUBRESOURCE SubResource {};

    assert(ViewportPos.x < m_fViewportWidth && ViewportPos.y < m_fViewportHeight && ViewportPos.x >= 0 && ViewportPos.y >= 0);

    _uint iPixelIndex = static_cast<_uint>(ViewportPos.y * m_fViewportWidth + ViewportPos.x);

    /* 텍스체에 값을 채운다. 수정 변경한다. */
    m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);
    _float2 vDepthDesc = static_cast<_float2*>(SubResource.pData)[iPixelIndex];
    m_pContext->Unmap(m_pTexture2D, 0);

    _vector			vWorldPos = XMVectorSet(ViewportPos.x, ViewportPos.y, 0.f, 0.f);

    vWorldPos = XMVectorSetX(vWorldPos, ViewportPos.x / (m_fViewportWidth * 0.5f) - 1.f);
    vWorldPos = XMVectorSetY(vWorldPos, ViewportPos.y / (m_fViewportHeight * -0.5f) + 1.f);
    vWorldPos = XMVectorSetZ(vWorldPos, vDepthDesc.x);
    vWorldPos = XMVectorSetW(vWorldPos, 1.f);

    vWorldPos = XMVector3TransformCoord(vWorldPos, m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::PROJ));
    vWorldPos = XMVector3TransformCoord(vWorldPos, m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW));

    return vWorldPos;
}

_bool CPixelPicking::Compute_ObjectID_WorldPos(const _float2& vViewportPos, _uint& Out)
{
    //if (false == m_pGameInstance->Check_IsAppFocused())
    //    return false;

    _float2 vViewport = vViewportPos;
    if (Is_OutOfBoundary(vViewport))
        return false;

    D3D11_MAPPED_SUBRESOURCE SubResource{};

    _uint iPixelIndex = static_cast<_uint>(vViewport.y * m_fViewportWidth + vViewport.x);

    m_pContext->Map(m_pTextureObjectID, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);
    _uint iID = static_cast<_uint*>(SubResource.pData)[iPixelIndex];

    m_pContext->Unmap(m_pTextureObjectID, 0);

    Out = iID;
    return (0 != iID);
}

_bool CPixelPicking::Is_OutOfBoundary(_float2 vMouse) const
{
    return 0.f > vMouse.x || m_fViewportWidth < vMouse.x || 0.f > vMouse.y || m_fViewportHeight < vMouse.y;
}

CPixelPicking* CPixelPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPixelPicking* pInstance = new CPixelPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Create : CPixelPicking");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPixelPicking::Free()
{
    super::Free();

    Safe_Release(m_pTexture2D);
    Safe_Release(m_pTextureObjectID);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
