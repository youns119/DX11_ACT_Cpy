#include "pch.h"
#include "Magnet.h"
#include "GameInstance.h"

CMagnet::CMagnet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CMagnet::CMagnet(const CMagnet& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CMagnet::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMagnet::Initialize(void* _pArg)
{
    DESC* pDesc = static_cast<DESC*>(_pArg);
    //pDesc->TransformDesc.fSpeedPerSec = 5.f;
    m_strName = (pDesc->GameObjectDesc).strName;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    {
        _wstring szFile = L"../Bin/DataFiles/Map_Object/MagnetRail.data"; //다이얼로그로 가져오는 파일패스 저장할 곳

        //로드하기
        _bool bRead = { false };
        HRESULT hr{};
        DWORD dwByte{};
        HANDLE hFile = CreateFile(szFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MSG_BOX("Can Not Open File");
        }

        while (true)
        {
            _float3 vPoint = {};
            bRead = ReadFile(hFile, &vPoint, sizeof(_float3), &dwByte, nullptr);
            if (!bRead || dwByte == 0)
                break;
            m_ControlPoint.push_back(vPoint);
        }

        CloseHandle(hFile);
    }

    m_fSparkTime = 0.15f;
    if (FAILED(Ready_Effects()))
        return E_FAIL;

    m_pGameInstance->Play_Sound_Loop(L"Magnet_Loop.wav", (_uint)SOUND_CHANNEL::MAPOBJECT_LOOP, 0.f);
    m_pGameInstance->Play_Sound_Loop(L"Magnet LoopIdle.wav", (_uint)SOUND_CHANNEL::MAPOBJECT_LOOP, 0.f);
    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::MAPOBJECT_LOOP, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 256.f), _vec2(0.f, 0.8f));

    return S_OK;
}

void CMagnet::Priority_Update(_float _fTimeDelta)
{
}

void CMagnet::Update(_float _fTimeDelta)
{
    if (m_bRender)
    {

        if (m_ControlPoint.size() < 4)
            return;

        if (m_bPaused)
        {
            m_fElapsedTime += _fTimeDelta;
            if (m_fElapsedTime >= m_fPausedTime)
            {
                m_bPaused = false;
                m_fElapsedTime = 0.0f;
            }
        }
        else
        {
            Call_Spark(_fTimeDelta);
            m_fInterpolationRate += _fTimeDelta * m_fSpeed;
            if (m_fInterpolationRate > 1.0f)
            {
                m_fInterpolationRate -= 1.0f; // 보간율 유지
                m_iSegmentIndex += (m_bFoward ? 1 : -1);

                if (m_bFoward && m_iSegmentIndex >= m_ControlPoint.size() - 3)
                {
                    m_iSegmentIndex = (_uint)m_ControlPoint.size() - 1;
                    m_bFoward = false;
                    m_bPaused = true; // 정지
                    m_fElapsedTime = 0.0f;
                }
                else if (!m_bFoward && m_iSegmentIndex - 2 <= 0)
                {
                    m_iSegmentIndex = 0;
                    m_bFoward = true;
                    m_bPaused = true; // 정지
                    m_fElapsedTime = 0.0f;
                }
            }
            _float3 vNewPosition = {};
            if (m_bFoward)
            {
                // 현재 구간에서 Catmull-Rom 보간
                vNewPosition = Catmull_rom(
                    m_ControlPoint[m_iSegmentIndex],
                    m_ControlPoint[m_iSegmentIndex + 1],
                    m_ControlPoint[m_iSegmentIndex + 2],
                    m_ControlPoint[m_iSegmentIndex + 3],
                    m_fInterpolationRate);
            }
            else
            {
                vNewPosition = Catmull_rom(
                    m_ControlPoint[m_iSegmentIndex],
                    m_ControlPoint[m_iSegmentIndex - 1],
                    m_ControlPoint[m_iSegmentIndex - 2],
                    m_ControlPoint[m_iSegmentIndex - 3],
                    m_fInterpolationRate);
            }

            m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vNewPosition));
        }
    }
    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::MAPOBJECT_LOOP, m_pTransformCom->Get_State(CTransform::STATE::POSITION), _vec2(1.f, 256.f), _vec2(0.f, 1.f));

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMagnet::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 40.f))
    {// 절두체 안에 있을 때만 컬링 

        _float fMaxRange = m_pGameInstance->Get_FogParams().vRange.y;
        super::Compute_CamDistance();

        if (fMaxRange > m_fCamDistance + 40.f)
        {
            m_bRender = true;
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

#ifdef _DEBUG
            m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif
        }
    }
    else
    {
        //m_bIsIn = false;
        m_bRender = false;
    }


#ifdef _DEBUG	
    //for (_int i = 0; i < (_int)m_ControlPoint.size(); i++)
    //{
    //    _vec4 vColor = (i == m_iSegmentIndex) ? _vec4(1.f, 0.f, 0.f, 1.f) : _vec4(0.f, 1.f, 0.f, 1.f);
    //    m_pGameInstance->Add_DebugRender_Sphere(m_ControlPoint[i], 0.2f, vColor);
    //}

    //// DebugComponent 등록
    //m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif
}

HRESULT CMagnet::Render()
{
    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // 즁요 ㅋㅋ
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        _uint iMaterialIndex = m_pModelCom->Get_MaterialIndex_In_Mesh(i);
        _uint iShaderPass = m_pModelCom->Get_ShaderPass(iMaterialIndex);

        if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
        {
            iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
                return E_FAIL;
        }
        else
        {
            iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

            if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
            return E_FAIL;

        m_pShaderCom->Begin(iShaderPass);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CMagnet::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix_Ref())))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_ShadowMatrices_To_Shader(m_pShaderCom, "g_LightViewMatrices", "g_LightProjMatrices")))
        return E_FAIL;

    for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_pShaderCom->Begin((_uint)SHADER_PASS_MESH::SHADOW);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

_float3 CMagnet::Catmull_rom(_float3 vPos0, _float3 vPos1, _float3 vPos2, _float3 vPos3, _float fInterpolationRate)
{
    _float fInterpolationRate2 = fInterpolationRate * fInterpolationRate;
    _float fInterpolationRate3 = fInterpolationRate2 * fInterpolationRate;

    return XMFLOAT3(
        0.5f * ((2.0f * vPos1.x) + (-vPos0.x + vPos2.x) * fInterpolationRate +
            (2.0f * vPos0.x - 5.0f * vPos1.x + 4.0f * vPos2.x - vPos3.x) * fInterpolationRate2 +
            (-vPos0.x + 3.0f * vPos1.x - 3.0f * vPos2.x + vPos3.x) * fInterpolationRate3),
        0.5f * ((2.0f * vPos1.y) + (-vPos0.y + vPos2.y) * fInterpolationRate +
            (2.0f * vPos0.y - 5.0f * vPos1.y + 4.0f * vPos2.y - vPos3.y) * fInterpolationRate2 +
            (-vPos0.y + 3.0f * vPos1.y - 3.0f * vPos2.y + vPos3.y) * fInterpolationRate3),
        0.5f * ((2.0f * vPos1.z) + (-vPos0.z + vPos2.z) * fInterpolationRate +
            (2.0f * vPos0.z - 5.0f * vPos1.z + 4.0f * vPos2.z - vPos3.z) * fInterpolationRate2 +
            (-vPos0.z + 3.0f * vPos1.z - 3.0f * vPos2.z + vPos3.z) * fInterpolationRate3)
    );
}

void CMagnet::Call_Spark(_float fTimeDelta)
{
    if (false == m_bPaused)
    {
        // ENV_MAGNET_SPARK
        m_fSparkTimeAcc += fTimeDelta;
        if (m_fSparkTimeAcc >= m_fSparkTime)
        {
            m_fSparkTimeAcc = 0.f;
            _vector vPos{}, vLook{};
            vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
            vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
            m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_MAGNET_SPARK, vPos, vLook, false, nullptr);
        }
    }
}

HRESULT CMagnet::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Magnet",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))

    {
        if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_Magnet",
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

    /* Com_Collider */
    {
        CCollider_AABB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.vExtents = { 10.f, 25.f, 10.f };
        Desc.vCenter = { 0.f, -30.f, 0.f };
        Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MAGNET; // 나중에 바꾸거나 할고.

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CMagnet::Ready_Effects()
{
    _vector vZeroPos{ 0.f, 0.f, 0.f, 1.f }, vZeroLook{ 0.f, 0.f, 1.f, 0.f };
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_MAGNET_MESH_PULL, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
    vZeroPos = XMVectorSetY(vZeroPos, -20.f);
    m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_MAGNET_MESHINST_SCRAP, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());

    return S_OK;
}

CMagnet* CMagnet::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMagnet* pInstance = new CMagnet(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CMagnet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CMagnet::Clone(void* _pArg)
{
    CMagnet* pInstance = new CMagnet(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CMagnet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMagnet::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}