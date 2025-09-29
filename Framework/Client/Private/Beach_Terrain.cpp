#include "pch.h"
#include "Beach_Terrain.h"

#include "GameInstance.h"

CBeach_Terrain::CBeach_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CBeach_Terrain::CBeach_Terrain(const CBeach_Terrain& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CBeach_Terrain::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBeach_Terrain::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC desc{};

    if (FAILED(super::Initialize(&desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBeach_Terrain::Priority_Update(_float fTimeDelta)
{
}

void CBeach_Terrain::Update(_float fTimeDelta)
{
}

void CBeach_Terrain::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysics_Terrain);
#endif
}

HRESULT CBeach_Terrain::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // �O�� ����
        return E_FAIL;

    HRESULT hr{};

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

HRESULT CBeach_Terrain::Load_HeightMap(const _tchar* pHeightMapFile)
{
    return m_pVIBufferCom->Load_HeightMap(pHeightMapFile);
}

HRESULT CBeach_Terrain::Ready_Components()
{
    /* Com_Texture_Diffuse */
    if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::DIFFUSE]))))
        return E_FAIL;


    {// �������� �븻 �Žñ� �ϱ� 
       /* Com_Texture_Normal*/
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal"),
            TEXT("Com_Texture_Normal"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::NORMAL]))))
            return E_FAIL;

        /* Com_Texture_Normal_Second */
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal_Wavy"), // ���� ���̺ꤿ
            TEXT("Com_Texture_Normal_Second"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::SECOND_NORMAL]))))
            return E_FAIL;

        /* Com_Texture_Normal_Third */
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal_Sand"), // �򻡽��
            TEXT("Com_Texture_Normal_Third"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::THIRD_NORMAL]))))
            return E_FAIL;

        /* Com_Texture_MetalicSmoothness */
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MS_Sand"), //��Ż���������Ͻ�
            TEXT("Com_Texture_MetalicSmoothness"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::MSEG]))))
            return E_FAIL;

        /* Com_Shader */
        if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
            TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;

        /* Com_VIBuffer */
        if (FAILED(super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_VIBuffer_Terrain_Beach"),
            TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
            return E_FAIL;
    }

    /* Com_Physics */
    {
        CPhysics_Terrain::DESC Desc{};
        Desc.pVIBuffer_Terrain = m_pVIBufferCom;
        Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        Desc.isInChunk = true;
        Desc.PhysicsDesc.fStaticFriction = 0.2f;
        Desc.PhysicsDesc.fDynamicFriction = 0.3f;

        if (FAILED(super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Physics_Terrain_Beach"),
            TEXT("Com_Physics"), reinterpret_cast<CComponent**>(&m_pPhysics_Terrain), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBeach_Terrain::Bind_ShaderResources()
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

	return S_OK;
}

CBeach_Terrain* CBeach_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBeach_Terrain* pInstance = new CBeach_Terrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CBeach_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CBeach_Terrain::Clone(void* pArg)
{
    CBeach_Terrain* pInstance = new CBeach_Terrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CBeach_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBeach_Terrain::Free()
{
    super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pPhysics_Terrain);

    for (auto& pTextureCom : m_TextureComs)
        Safe_Release(pTextureCom);
}
