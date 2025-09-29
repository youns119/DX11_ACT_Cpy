#include "pch.h"
#include "ShellHouse.h"

#include "GameInstance.h"
#include "FileData_Locator.h"
#include "UI_Manager.h"
#include "EffectObject.h"

#include "Player.h"

map<_int, _bool> CShellHouse::s_ActivatedShells;

CShellHouse::CShellHouse(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CShellHouse::CShellHouse(const CShellHouse& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CShellHouse::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;

}

HRESULT CShellHouse::Initialize(void* _pArg)
{
    CShellHouse::DESC* pDesc = static_cast<CShellHouse::DESC*>(_pArg);
    pDesc->GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iSheleportID = pDesc->iSheleportID;

    m_tData.SheleportData = CFileData_Locator<FSheleportData>::Get_FileData()->Get_Data(0, m_iSheleportID);
    m_tData.iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
    XMStoreFloat3(&m_tData.vPosition, m_pTransformCom->Get_State(CTransform::STATE::POSITION));
    XMStoreFloat3(&m_tData.vLook, m_pTransformCom->Get_State(CTransform::STATE::LOOK));
    XMStoreFloat3(&m_tData.vRight, m_pTransformCom->Get_State(CTransform::STATE::RIGHT));

    if (true == Check_Activated())
        Change_State(ACTIVATED);
    else
        Change_State(WAIT);


    {/* PointLight */

        _vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
        vPos = XMVectorSetY(vPos,(XMVectorGetY(vPos) + 2.f));


        LIGHT_DESC LightDesc = {};
        LightDesc.vDirection = _float4(0.f, 1.f, 0.05f, 0.f);
        LightDesc.vCosCutOff = _float2(50.f, 80.f);


        LightDesc.eType = LIGHT_TYPE::SPOT;
        LightDesc.fRange = 80.f;
        XMStoreFloat4(&LightDesc.vPosition, vPos);
        LightDesc.vDiffuse = _float4(5.4f, 2.f, 4.2f, 1.f);
        LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
        //LightDesc.vSpecular = m_vSpecular;
        LightDesc.vAttenuation = _float3(1.f, 0.2461f, 0.9f);

        _wstring strLightTag = L"Light_ShellHouse" + to_wstring(m_iID);
        m_pGameInstance->Add_Light(strLightTag, LightDesc);

    }

    return S_OK;
}

void CShellHouse::Priority_Update(_float _fTimeDelta)
{
}

void CShellHouse::Update(_float _fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
    Call_Effect_Trigger(_fTimeDelta);
}

void CShellHouse::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 10.f))
    {// 절두체 안에 있을 때만 컬링 

        _float fMaxRange = m_pGameInstance->Get_FogParams().vRange.y;
        super::Compute_CamDistance();

        if (fMaxRange > m_fCamDistance + 10.f)
        {
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);
#ifdef _DEBUG
            m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

            //if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Get_Count() > 0)
            if (nullptr != m_pPhysicsCom && m_pPhysicsCom->Is_Simulated())
            {
                m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsCom);
            }
#endif // _DEBUG
        }
    }
}

HRESULT CShellHouse::Render()
{

    if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // 즁요 ㅋㅋ
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
        return E_FAIL;

    if (ACTIVATED == m_eState)
    {
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
    }    

    return S_OK;
}

void CShellHouse::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

        // 활성화 된 애인지 확인
        if (true == Check_Activated())
            pUIManager->Show_Interaction(L"들어가기");
        else
            pUIManager->Show_Interaction(L"해류 건드리기");
    }
    break;
    default:
        break;
    }
}

void CShellHouse::On_Collision(CGameObject* pOther, _uint iGroup)
{
    switch (iGroup)
    {
    case (_uint)COLLISION_GROUP::PLAYER:
    {
        // 활성화 애니메이션이 끝날 때까지 상호작용 불가능
        if (m_eState != ACTIVATING)
        {
            if (m_pGameInstance->Key_Down(KEY::E))
            {
                CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

                if (false == Check_Activated())
                {
                    m_pGameInstance->Play_Sound(L"MSS_Spawn_New.wav", (_uint)SOUND_CHANNEL::SHELLHOUSE, 1.f);
                    // 상호작용 불가능 상태 진입
                    Change_State(ACTIVATING);
                    pUIManager->Hide_Interaction();
                    pUIManager->Add_Visited(m_tData);
                    pUIManager->Show_TextUI(L"골뱅이 껍데기 발견함", _vec4(_vec4(235.f, 215.f, 254.f, 255.f) / 255.f)._float4());
                }
                else
                {
                    pUIManager->Hide_Interaction();
                    pUIManager->Set_MoonSnailUI(m_tData);
                    pUIManager->Show_MoonSnailUI();
                    m_pGameInstance->Play_Sound(L"MSS_Enter.wav", (_uint)SOUND_CHANNEL::SHELLHOUSE, 1.f);
                }

                CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
                if (pPlayer == nullptr)
                    return;

                pPlayer->Clear_Buffer();
            }
        }
        // 진입한 상태에서 Activating 끝나면 들어가기 버튼 켜야함
    }
    break;
    default:
        break;
    }
}

void CShellHouse::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
    pUIManager->Hide_Interaction();
}

_bool CShellHouse::Check_Activated()
{
    if (s_ActivatedShells.find(m_iSheleportID) == s_ActivatedShells.end())
        s_ActivatedShells.insert({ m_iSheleportID, false });

    return s_ActivatedShells.find(m_iSheleportID)->second;
}

void CShellHouse::Change_State(SHELLSTATE eState)
{
    if (eState != m_eState)
        m_eState = eState;
    else
        return;

    _vector vPos{}, vLook{}, vUp{};
    vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK) * -1.f;
    vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);

    switch (m_eState)
    {
    case WAIT:
        vPos = vPos + vUp * -3.5f;
        m_pEffects[WAIT_WIND00] = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND_WAIT00, vPos, vLook, false, nullptr);
        m_pEffects[WAIT_WIND01] = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND_WAIT01, vPos, vLook, false, nullptr);
        m_pEffects[WAIT_CIRCLE] = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_SPRT_CIRCLE_WAIT, vPos, vLook, false, nullptr);
        break;
    case ACTIVATING:
        vPos += vUp * 0.45f;
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_SHELLHOUSE, vPos, -vLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_SHELLBRICK, vPos, -vLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_BARNACLE, vPos, -vLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND_ACTIVATED, vPos, -vLook, false, nullptr);
        m_pColliderCom->Set_Active(false);
        break;
    case ACTIVATED:
        vPos = vPos + vUp * -5.f;
        if (s_ActivatedShells.find(m_iSheleportID) != s_ActivatedShells.end())
            s_ActivatedShells.find(m_iSheleportID)->second = true;
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND00, vPos, vLook, false, nullptr);
        m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::GOLEPORT_MESH_WIND01, vPos, vLook, false, nullptr);
        m_pColliderCom->Set_Active(true);
        break;
    default:
        break;
    }
}

void CShellHouse::Call_Effect_Trigger(_float fTimeDelta)
{
    _bool bCheckTrigger{ false };
    switch (m_eState)
    {
    case ACTIVATING:
        if (nullptr != m_pEffects[WAIT_WIND00])
            bCheckTrigger = m_pEffects[WAIT_WIND00]->Call_Trigger_Event(fTimeDelta);
        if (nullptr != m_pEffects[WAIT_WIND01])
            m_pEffects[WAIT_WIND01]->Call_Trigger_Event(fTimeDelta);
        if (nullptr != m_pEffects[WAIT_CIRCLE])
            m_pEffects[WAIT_CIRCLE]->Call_Trigger_Event(fTimeDelta);

        if (true == bCheckTrigger)
        {
            m_pEffects[WAIT_CIRCLE]->Clear_Info(fTimeDelta);
            Change_State(ACTIVATED);
        }
        break;
    default:
        break;
    }
}

HRESULT CShellHouse::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MoonShell"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    {
        CCollider_AABB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::PORTAL);

        Desc.vExtents = { 10.0f, 10.0f,  10.0f };
        Desc.vCenter = { 0.f, Desc.vExtents.y * 0.5f, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    {
        CPhysics_Mesh::DESC Desc {};
        Desc.PhysicsDesc.fStaticFriction = 0.8f;
        Desc.PhysicsDesc.fDynamicFriction = 0.7f;
        Desc.PhysicsDesc.fRestitution = 0.2f;
        Desc.PhysicsDesc.isStatic = true;
        Desc.PhysicsDesc.iFilter = (_uint)PHYSICS_FILTER::DEFAULT;
        Desc.PhysicsDesc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());

        Desc.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        Desc.isInChunk = true;

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_MoonShell",
            L"Com_Physics", reinterpret_cast<CComponent**>(&m_pPhysicsCom), &Desc)))
            return E_FAIL;    
    }

    return S_OK;
}

CShellHouse* CShellHouse::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CShellHouse* pInstance = new CShellHouse(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CShellHouse");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CShellHouse::Clone(void* _pArg)
{
    CShellHouse* pInstance = new CShellHouse(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CShellHouse");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShellHouse::Free()
{
    super::Free();
    
    m_pGameInstance->Remove_Light(L"Light_ShellHouse" + to_wstring(m_iID));

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pPhysicsCom);
}
