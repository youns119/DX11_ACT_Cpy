#include "pch.h"
#include "Radio.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

CRadio::CRadio(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : super{ _pDevice, _pContext }
{
}

CRadio::CRadio(const CRadio& _Prototype)
    : super{ _Prototype }
{
}

HRESULT CRadio::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRadio::Initialize(void* _pArg)
{
    DESC* pDesc = static_cast<DESC*>(_pArg);
    //pDesc->TransformDesc.fSpeedPerSec = 5.f;
    m_strName = (pDesc->GameObjectDesc).strName;

    if (FAILED(super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pGameInstance->Play_Sound_Loop(L"Club.wav", (_uint)SOUND_CHANNEL::BGM, 0.f);

    // BGM 채널 얻기
    //FMOD::Channel* pBGMChannel = m_pGameInstance->Get_Channel((_uint)SOUND_CHANNEL::BGM);

    //if (!pBGMChannel) 
        //return E_FAIL;

    // DSP 디지털 신호 처리 저음가튼거 분석해서 저음고음 세기 가튼거 알수 있음 노래에 맞춰서 둥둥 하려면 필요험
    //if (!m_pDSP)
    //{
    //    m_pDSP = m_pGameInstance->Get_BGM_DSP();
    //    m_pDSP->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, 1024);
    //    pBGMChannel->addDSP(0, m_pDSP);
    //}

    //m_pChannel = pBGMChannel; // 필요하면 멤버로 저장

    return S_OK;
}

void CRadio::Priority_Update(_float _fTimeDelta)
{
}

void CRadio::Update(_float _fTimeDelta)
{
    m_fAccTimeDelta += _fTimeDelta;

    _float fBPM = 65.f;
    _float fBeatPerSecond = fBPM / 60.f;
    _float fPulse = sinf(m_fAccTimeDelta * XM_2PI * fBeatPerSecond);
    _float fScale = 1.14f + powf(fPulse, 8.f) * 0.1f;

    //m_pTransformCom->Set_Scale(1.14f, fScale, 1.14f);
    m_pTransformCom->Set_Scale(fScale, fScale, fScale);

    m_fEffectTimeAcc += _fTimeDelta;
    if (m_fEffectTime <= m_fEffectTimeAcc)
        Call_MusicEffect();
    //if (m_pDSP)
    //{
    //    // 주파수 스펙트럼 데이터 가져오기
    //    FMOD_DSP_PARAMETER_FFT* pFFT = nullptr;
    //    unsigned int fftLength = 0;

    //    // getParameterData 호출 (실제로 이 데이터를 받아오는 코드)
    //    FMOD_RESULT result = m_pDSP->getParameterData(
    //        FMOD_DSP_FFT_SPECTRUMDATA,
    //        (void**)&pFFT,
    //        &fftLength,
    //        nullptr,
    //        0
    //    );

    //    if (result == FMOD_OK && pFFT && pFFT->length > 0)
    //    {
    //        // 저음 대역 (예: 첫 번째 주파수 대역 사용)
    //        _float fBass = pFFT->spectrum[0][0];  // 0번 인덱스 = 저음 영역

    //        // 저음에 맞춰 크기 변화
    //        _float fBassScale = 1.0f + fBass * 0.1f;  // 저음민감도 

    //        if (fBassScale > 1.5f)
    //            fBassScale = 1.5f;
    //        if (fBassScale < 1.0f)
    //            fBassScale = 1.0f;

    //        // 크기 적용
    //        m_pTransformCom->Set_Scale(fBassScale, fBassScale, fBassScale);
    //    }
    //}
    _vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
    m_pGameInstance->Set_ChannelVolume_With_SpatialDesc((_uint)SOUND_CHANNEL::BGM, vPos, _vec2(10.f, 140.f), _vec2(0.f, 1.f));
}

void CRadio::Late_Update(_float _fTimeDelta)
{
    if (true == m_pGameInstance->Is_InFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE::POSITION), 50.f))
    {// 절두체 안에 있을 때만 컬링 

        m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
        //m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this); // 어두워서 필요없을거같아서 없애놧어요
    }
}

HRESULT CRadio::Render()
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

HRESULT CRadio::Render_Shadow()
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

void CRadio::Call_MusicEffect()
{
    m_fEffectTimeAcc = 0.f;

    CEffectObject* pEffect = nullptr;
    _vector vPos{}, vRight{}, vUp{}, vLook{}, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) }, vCamPos{}, vCamLook{};
    vRight = m_pTransformCom->Get_State(CTransform::STATE::RIGHT) * 4.f;
    vUp = m_pTransformCom->Get_State(CTransform::STATE::UP) * 4.f;
    vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK) * 2.f;
    vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION) + vUp + vRight + vLook * 0.5f;
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_MUSIC_ON, vPos, vZeroLook, false, nullptr);
    if(nullptr != pEffect)
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot( (vRight + vUp * 0.5f) * -5.f);
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_MUSIC_QUATER, vPos, vZeroLook, false, nullptr);
    if (nullptr != pEffect)
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot( (vRight + vUp * 0.5f) * -5.f);
    pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENV_MUSIC_LINE, vPos, vZeroLook, false, nullptr);
    if (nullptr != pEffect)
        static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot((vRight + vUp * 0.5f) * -5.f);

    vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
    vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
    CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook * 2.f, vCamLook, false, nullptr);
    
}

HRESULT CRadio::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Radio",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))

    {
        if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), L"Prototype_Component_Model_Radio",
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

    return S_OK;
}

CRadio* CRadio::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRadio* pInstance = new CRadio(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CRadio");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CRadio::Clone(void* _pArg)
{
    CRadio* pInstance = new CRadio(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Clone : CRadio");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRadio::Free()
{
    super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
