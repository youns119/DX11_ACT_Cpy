#include "pch.h"
#include "Level_Tool.h"

#include "GameInstance.h"
#include "Sky.h"

#include "AnimObject.h"
#include "ToolCamera.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel{ _pDevice, _pContext }
{
}

HRESULT CLevel_Tool::Initialize()
{
    if (FAILED(Ready_Lights()) ||
        FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))) ||
        FAILED(Ready_Environment(TEXT("Layer_Environment"))) ||
        FAILED(Ready_Layer_AnimObject(TEXT("Layer_AnimObject"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Tool::Update(_float _fTimeDelta)
{
}

HRESULT CLevel_Tool::Render()
{
    return S_OK;
}

HRESULT CLevel_Tool::Ready_Lights()
{
    if (FAILED(m_pGameInstance->SetUp_ShadowLight
    (
        XMVectorSet(-10.f, 10.f, -10.f, 1.f),
        XMVectorSet(0.f, 0.f, 0.f, 1.f),
        XMConvertToRadians(60.f),
        g_iWindowSizeX / static_cast<_float>(g_iWindowSizeY),
        0.1f, 1000.f
    )))
        return E_FAIL;

    LIGHT_DESC LightDesc{};
    ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));

    LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
    LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
    LightDesc.vDiffuse = _float4(2.0f, 2.0f, 2.0f, 2.0f);
    //LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
    LightDesc.vAmbient = _float4(1.0f, 1.0f, 1.0f, 1.f); // for debug

    if (FAILED(m_pGameInstance->Add_Light(L"Light_Global", LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const _wstring& _strLayerTag)
{
    CToolCamera::DESC ToolCameraDesc{};
    {
        ToolCameraDesc.CameraDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 10.f;
        ToolCameraDesc.CameraDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

        ToolCameraDesc.CameraDesc.vEye = _float3(7.f, 4.f, 7.f);
        ToolCameraDesc.CameraDesc.vAt = _float3(0.f, 1.f, 0.f);
        ToolCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
        ToolCameraDesc.CameraDesc.fNear = 0.1f;
        ToolCameraDesc.CameraDesc.fFar = 1000.f;

        //ToolCameraDesc.fMouseSensitivity = 0.005f;
        ToolCameraDesc.fMouseSensitivity = 0.1f;
    }

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_TOOL,
        TEXT("GameObject_ToolCamera"),
        LEVEL_TOOL,
        _strLayerTag,
        &ToolCameraDesc
    )))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Environment(const _wstring& _strLayerTag)
{
    // Sky
    CSky::DESC SkyDesc{};
    SkyDesc.iTextureIndex = 3;
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVELID::LEVEL_TOOL, 
        L"GameObject_Sky", 
        LEVELID::LEVEL_TOOL,
        _strLayerTag,
        &SkyDesc
    )))
        return E_FAIL;

    // Terrain
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_TOOL, 
        TEXT("GameObject_Terrain"),
        LEVEL_TOOL,
        _strLayerTag
    )))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_AnimObject(const _wstring& _strLayerTag)
{
    CAnimObject::DESC AnimObjectDesc{};
    AnimObjectDesc.strModelTag = L"Model_Player";
    AnimObjectDesc.eLevelID = LEVEL_TOOL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer
    (
        LEVEL_TOOL,
        TEXT("GameObject_AnimObject"),
        LEVEL_TOOL,
        _strLayerTag,
        &AnimObjectDesc
    )))
        return E_FAIL;

    return S_OK;
}

void CLevel_Tool::SetUp_CollisionGroup()
{
    m_pGameInstance->Set_CollisionEnableGroup((_uint)COLLISION_GROUP::CINECAMERA, (_uint)COLLISION_GROUP::CAMERADEBUG, true);
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_Tool* pInstance = new CLevel_Tool(_pDevice, _pContext);

    return pInstance;
}

void CLevel_Tool::Free()
{
    __super::Free();
}