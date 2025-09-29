#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC desc{};

    if (FAILED(super::Initialize(&desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
    //m_pNavigationCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CTerrain::Update(_float fTimeDelta)
{
}

void CTerrain::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysics_Terrain);
    //m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::BLEND, m_pNavigationCom);
#endif
}

HRESULT CTerrain::Render()
{
    //이거 넣으니까 IMGUI 가 출력이 안됨 다녀와서 보기
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // 즁요 ㅋㅋ
        return E_FAIL;

    HRESULT hr {};

    hr = Bind_ShaderResources();
    if (FAILED(hr))
        return E_FAIL;

    hr = m_pShaderCom->Begin(0); // 0 : default , 1 : brush
    if (FAILED(hr))
        return E_FAIL;

    hr = m_pVIBufferCom->Bind_InputAssembler();
    if (FAILED(hr))
        return E_FAIL;

    hr = m_pVIBufferCom->Render();
    if (FAILED(hr))
        return E_FAIL;



    return S_OK;
}
HRESULT CTerrain::Load_HeightMap(const _tchar* pHeightMapFile)
{
    return m_pVIBufferCom->Load_HeightMap(pHeightMapFile);
}

HRESULT CTerrain::Ready_Components()
{
    /* Com_Texture_Diffuse */
    if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::DIFFUSE]))))
        return E_FAIL;


    {// 레벨별로 노말 거시기 하기 
       /* Com_Texture_Normal*/
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal"),
            TEXT("Com_Texture_Normal"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::NORMAL]))))
            return E_FAIL;

        /* Com_Texture_Normal_Second */
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal_Wavy"), // 약한 웨이브ㅏ
            TEXT("Com_Texture_Normal_Second"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::SECOND_NORMAL]))))
            return E_FAIL;

        /* Com_Texture_Normal_Third */
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal_Sand"), // 흰빨쎈거
            TEXT("Com_Texture_Normal_Third"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::THIRD_NORMAL]))))
            return E_FAIL;

        /* Com_Texture_Mask*/
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain_Mask"),
            TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::MASK]))))
            return E_FAIL;

        /* Com_Texture_MetalicSmoothness */
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MS_Sand"), //메탈릭스무스니스
            TEXT("Com_Texture_MetalicSmoothness"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::MSEG]))))
            return E_FAIL;
    }


    /* Com_Shader */
    if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    ///* Com_Navigation */
    ////if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation"),
    //if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation_TriangluationTest"),
    //   TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
    //   return E_FAIL;

    /* Com_Physics */
    {
        CPhysics_Terrain::DESC Desc {};
        Desc.pVIBuffer_Terrain = m_pVIBufferCom;
        Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        Desc.isInChunk = true;
        Desc.PhysicsDesc.fStaticFriction = 0.2f;
        Desc.PhysicsDesc.fDynamicFriction = 0.3f;

        if (FAILED(super::Add_Component(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Physics_Terrain_Field"),
            TEXT("Com_Physics"), reinterpret_cast<CComponent**>(&m_pPhysics_Terrain), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
        m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
        m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::DIFFUSE]->Bind_Textures_To_Shader(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;
    if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::NORMAL]->Bind_Texture_To_Shader(m_pShaderCom, "g_NormalTexture"))) // Texture
        return E_FAIL;
    if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::SECOND_NORMAL]->Bind_Texture_To_Shader(m_pShaderCom, "g_SecondNormalTexture"))) // Texture
        return E_FAIL;
    if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::THIRD_NORMAL]->Bind_Texture_To_Shader(m_pShaderCom, "g_ThirdNormalTexture"))) // Texture
        return E_FAIL;
    if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::MSEG]->Bind_Texture_To_Shader(m_pShaderCom, "g_MSTexture"))) // Texture
        return E_FAIL;

    _float fMaskDensity = 70.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskDensity", &fMaskDensity, sizeof(_float))))
        return E_FAIL;

    
    return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain* pInstance = new CTerrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CTerrain::Clone(void* pArg)
{
    CTerrain* pInstance = new CTerrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CTerrain::Free()
{
    super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pPhysics_Terrain);

    for (auto& pTextureCom : m_TextureComs)
        Safe_Release(pTextureCom);
}
