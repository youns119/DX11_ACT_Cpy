#include "Shadow.h"

#include "GameInstance.h"

CShadow::CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CShadow::Initialize()
{
    return S_OK;
}

void CShadow::Update()
{
    /* Light Dir 이 정의되어 있지 않은 경우 조기 리턴, 씬 변경될때 좀 살펴봐야겟다. */
    if (XMVector3Equal(m_vLightDir._vector(), XMVectorZero()))
        return;

    array<_vec3, 8> CameraFrustum {};
#ifdef SHADOW_DEBUG
    //CameraFrustum = Create_TestFrustum();
    if (m_pGameInstance->Key_Down(KEY::P))
        Save_ShadowDebugFrustum();

    CameraFrustum = m_ShadowDebugFrustum;
#else
    const array<_float4, 8>& Points = m_pGameInstance->Get_FrustumWorldPoints();
    for (_uint i = 0; i < 8; ++i)
    {
        CameraFrustum[i] = _vec3(Points[i]);
    }
#endif //  SHADOW_DEBUG


    _float Radiuses[3] {};
    _vec3 CenterPositions[3] {};
    _float IntervalRatio[3] { 0.03f, 0.15f, 0.3f };
    _vec4 Colors[3] { {1.f, 0.f, 0.f, 1.f}, {1.f, 0.5f, 0.f, 1.f}, {1.f, 1.f, 0.f, 1.f} };

    vector<array<_vec3, 8>> CascadedFrustums;
    CascadedFrustums.reserve(s_iNumCascade + 1);
    /* 생성을 위해 임시로 채워 넣음 */
    CascadedFrustums.push_back({ _vec3{}, _vec3{}, _vec3{}, _vec3{}, CameraFrustum[0], CameraFrustum[1], CameraFrustum[2], CameraFrustum[3] });


    //for (auto& Frustum : CascadedFrustums)
    for (_uint i = 0; i < s_iNumCascade; ++i)
    {
        array<_vec3, 8> Frustum {};
        for (_uint j = 0; j < 4; ++j)
        {
            const auto& LastFrustum = CascadedFrustums.back();
            Frustum[j] = LastFrustum[j + 4];
            Frustum[j + 4] = CameraFrustum[j] + IntervalRatio[i] * (CameraFrustum[j + 4] - CameraFrustum[j]);
        }

        Compute_CircumscribedSphere(CenterPositions[i], Radiuses[i], Frustum, Colors[i]);
        CascadedFrustums.push_back(Frustum);
    }
    /* 생성을 위해 임시로 채워 넣었던 카메라 절두체 제거 */
    CascadedFrustums.assign(CascadedFrustums.begin() + 1, CascadedFrustums.end());

    for (_uint i = 0; i < s_iNumCascade; ++i)
    {
        m_CircumscribeSpheres[i] = CenterPositions[i];
        m_CircumscribeSpheres[i].w = Radiuses[i];
    }

    for (_uint i = 0; i < s_iNumCascade; ++i)
    {
        XMStoreFloat4x4(&m_LightViewMatrices[i], XMMatrixLookAtLH(
            _vec3(CenterPositions[i] - m_vLightDir * (m_fFar * 0.5f))._vector(),
            CenterPositions[i]._vector(),
            XMVectorSet(0.f, 1.f, 0.f, 0.f)));
        XMStoreFloat4x4(&m_LightProjMatrices[i], XMMatrixOrthographicLH(Radiuses[i] * 2.f, Radiuses[i] * 2.f, m_fNear, m_fFar));
    }

#ifdef _DEBUG 
    auto Draw_Frustum = [](array<_vec3, 8> FrustumPoints, const _vec4& vColor)->void
        {
            for (_uint i = 0; i < 4; ++i)
            {
                _vec3 Quad[4] =
                {
                    _vec3(FrustumPoints[(i + 0) % 4]),
                    _vec3(FrustumPoints[(i + 1) % 4]),
                    _vec3(FrustumPoints[(i + 1) % 4 + 4]),
                    _vec3(FrustumPoints[(i + 0) % 4 + 4]),
                };
                CGameInstance::GetInstance()->Add_DebugRender_Quad(Quad, vColor);
            }
        };

    _vec4 FrustumColors[3] { {0.f, 0.f, 1.f, 1.f}, {0.5f, 1.f, 1.f, 1.f}, {1.f, 0.f, 1.f, 1.f} };
    for (_uint i = 0; i < 3; ++i)
        Draw_Frustum(CascadedFrustums[i], FrustumColors[i]);
#endif // _DEBUG
}

HRESULT CShadow::SetUp_ShadowLight(_fvector vEye, _fvector vAt, _float fLightAngle, _float fAspect, _float fNear, _float fFar)
{
    for (_uint i = 0; i < s_iNumCascade; ++i)
    {
        XMStoreFloat4x4(&m_LightViewMatrices[i], XMMatrixLookAtLH(vEye, vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
        //XMStoreFloat4x4(&m_LightProjMatrix, XMMatrixPerspectiveFovLH(fLightAngle, fAspect, fNear, fFar));
        XMStoreFloat4x4(&m_LightProjMatrices[i], XMMatrixOrthographicLH(1.f, 1.f, fNear, fFar));
    }

    m_vLightDir = _mat(m_LightViewMatrices[0]).Inverse().look;

    return S_OK;
}

HRESULT CShadow::SetUp_ShadowLight(_vec3 vLightDir, _float fNear, _float fFar)
{
    m_vLightDir = vLightDir.Normalized();
    m_fNear = fNear;
    m_fFar = fFar;

    return S_OK;
}

HRESULT CShadow::Bind_Matrices(CShader* pShader, const _char* pViewConstantName, const _char* pProjConstantName) const
{
    if (FAILED(pShader->Bind_Matrices(pViewConstantName, m_LightViewMatrices, s_iNumCascade)))
        return E_FAIL;

    if (FAILED(pShader->Bind_Matrices(pProjConstantName, m_LightProjMatrices, s_iNumCascade)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShadow::Bind_CascadeCircumSphere_To_Shader(CShader* pShader, const _char* pCircueSphereConstantName) const
{
    if (FAILED(pShader->Bind_RawValue(pCircueSphereConstantName, &m_CircumscribeSpheres, sizeof(_vec4) * s_iNumCascade)))
        return E_FAIL;

    return S_OK;
}

//void CShadow::Cal_Cuboid(const array<_vec4, 8>& FrustumPoints)
//{
//    _mat ViewMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LightViewMatrix));
//
//    _vec3 vMaxRight {}, vMinRight {};
//    _vec3 vMaxUp {}, vMinUp {};
//    _vec3 vMaxLook {}, vMinLook {};
//
//    //for (_uint i=0; i<static_cast<_uint>(FrustumPoints.size()); ++i)
//    for (_vec4 Point : FrustumPoints)
//    {
//        if (_vec4::Dot(ViewMatrix.right, Point) > _vec4::Dot(ViewMatrix.right, vMaxRight))
//        {
//            vMaxRight = Point;
//        }
//        if (_vec4::Dot(ViewMatrix.right, Point) < _vec4::Dot(ViewMatrix.right, vMinRight))
//        {
//            vMinRight = Point;
//        }
//
//        if (_vec4::Dot(ViewMatrix.up, Point) > _vec4::Dot(ViewMatrix.up, vMaxUp))
//        {
//            vMaxUp = Point;
//        }
//        if (_vec4::Dot(ViewMatrix.up, Point) < _vec4::Dot(ViewMatrix.up, vMinUp))
//        {
//            vMinUp = Point;
//        }
//
//        if (_vec4::Dot(ViewMatrix.look, Point) > _vec4::Dot(ViewMatrix.look, vMaxLook))
//        {
//            vMaxLook = Point;
//        }
//        if (_vec4::Dot(ViewMatrix.look, Point) < _vec4::Dot(ViewMatrix.look, vMinLook))
//        {
//            vMinLook = Point;
//        }
//    }
//    _float fMaxRight = _vec4::Dot(ViewMatrix.right, vMaxRight);
//    _float fMinRight = _vec4::Dot(ViewMatrix.right, vMinRight);
//    _float fMaxUp = _vec4::Dot(ViewMatrix.up, vMaxUp);
//    _float fMinUp = _vec4::Dot(ViewMatrix.up, vMinUp);
//    _float fMaxLook = _vec4::Dot(ViewMatrix.look, vMaxLook);
//    _float fMinLook = _vec4::Dot(ViewMatrix.look, vMinLook);
//
//    _vec4 vPlaneRight = XMPlaneFromPointNormal(vMaxRight._vector(), ViewMatrix.right._vector());
//    _vec4 vPlaneTop = XMPlaneFromPointNormal(vMaxUp._vector(), ViewMatrix.up._vector());
//    _vec4 vPlaneNear = XMPlaneFromPointNormal(vMaxLook._vector(), ViewMatrix.look._vector());
//
//    _vec4 vPlaneLeft = XMPlaneFromPointNormal(vMinRight._vector(), ViewMatrix.right._vector());
//    _vec4 vPlaneBottom = XMPlaneFromPointNormal(vMinUp._vector(), ViewMatrix.up._vector());
//    _vec4 vPlaneFar = XMPlaneFromPointNormal(vMinLook._vector(), ViewMatrix.look._vector());
//
//
//    _mat EquationMatrix {};
//    _vec4 PlaneD {};
//    EquationMatrix.r[0] = vPlaneRight;
//    EquationMatrix.r[1] = vPlaneTop;
//    EquationMatrix.r[2] = vPlaneNear;
//    swap(PlaneD.x, EquationMatrix.r[0].w);
//    swap(PlaneD.y, EquationMatrix.r[1].w);
//    swap(PlaneD.z, EquationMatrix.r[2].w);
//    //PlaneD = PlaneD * -1.f;
//
//    _mat EquationMatrixInverse = XMMatrixInverse(nullptr, EquationMatrix._matrix());
//    _vec4 vMaxPosition = PlaneD.Transform(EquationMatrixInverse);
//
//    PlaneD = {};
//    EquationMatrix.r[0] = vPlaneLeft;
//    EquationMatrix.r[1] = vPlaneBottom;
//    EquationMatrix.r[2] = vPlaneFar;
//    swap(PlaneD.x, EquationMatrix.r[0].w);
//    swap(PlaneD.y, EquationMatrix.r[1].w);
//    swap(PlaneD.z, EquationMatrix.r[2].w);
//    //PlaneD = PlaneD * -1.f;
//
//    EquationMatrixInverse = XMMatrixInverse(nullptr, EquationMatrix._matrix());
//    _vec4 vMinPosition = PlaneD.Transform(EquationMatrixInverse);
//
//
//    //_vec3 vCenter = (vMaxPosition - vMinPosition) * 0.5f + vMinPosition;
//    _vec3 vCenter {};
//    for (auto& Point : FrustumPoints)
//        vCenter = vCenter + Point;
//    vCenter = vCenter / 8.f;
//
//    _vec3 vExtents {};
//    vExtents.x = abs(fMaxRight - fMinRight);
//    vExtents.y = abs(fMaxUp - fMinUp);
//    vExtents.z = abs(fMaxLook - fMinLook);
//    vExtents = vExtents * 0.5f;
//
//    _vec3 vFitPoints[8] {};
//    vFitPoints[0] = vCenter - vExtents.x * ViewMatrix.right + vExtents.y * ViewMatrix.up - vExtents.z * ViewMatrix.look;
//    vFitPoints[1] = vCenter + vExtents.x * ViewMatrix.right + vExtents.y * ViewMatrix.up - vExtents.z * ViewMatrix.look;
//    vFitPoints[2] = vCenter + vExtents.x * ViewMatrix.right - vExtents.y * ViewMatrix.up - vExtents.z * ViewMatrix.look;
//    vFitPoints[3] = vCenter - vExtents.x * ViewMatrix.right - vExtents.y * ViewMatrix.up - vExtents.z * ViewMatrix.look;
//    vFitPoints[4] = vCenter - vExtents.x * ViewMatrix.right + vExtents.y * ViewMatrix.up + vExtents.z * ViewMatrix.look;
//    vFitPoints[5] = vCenter + vExtents.x * ViewMatrix.right + vExtents.y * ViewMatrix.up + vExtents.z * ViewMatrix.look;
//    vFitPoints[6] = vCenter + vExtents.x * ViewMatrix.right - vExtents.y * ViewMatrix.up + vExtents.z * ViewMatrix.look;
//    vFitPoints[7] = vCenter - vExtents.x * ViewMatrix.right - vExtents.y * ViewMatrix.up + vExtents.z * ViewMatrix.look;
//
//    for (_uint i = 0; i < 4; ++i)
//    {
//        _vec3 Quad[4] =
//        {
//            _vec3(vFitPoints[(i + 0) % 4]),
//            _vec3(vFitPoints[(i + 1) % 4]),
//            _vec3(vFitPoints[(i + 1) % 4 + 4]),
//            _vec3(vFitPoints[(i + 0) % 4 + 4]),
//        };
//        m_pGameInstance->Add_DebugRender(Quad, _vec4(1.f, 0.0f, 0.f, 1.f));
//    }
//    for (_uint i = 0; i < 2; ++i)
//    {
//        _vec3 Quad[4] =
//        {
//            _vec3(vFitPoints[4 * i + 0]),
//            _vec3(vFitPoints[4 * i + 1]),
//            _vec3(vFitPoints[4 * i + 2]),
//            _vec3(vFitPoints[4 * i + 3]),
//        };
//        m_pGameInstance->Add_DebugRender(Quad, _vec4(1.f, 0.0f, 0.f, 1.f));
//    }
//
//    m_pGameInstance->Add_DebugRender(vCenter, ViewMatrix.right, true, _vec4(1.f, 0.f, 0.f, 1.f));
//    m_pGameInstance->Add_DebugRender(vCenter, ViewMatrix.up, true, _vec4(0.f, 1.f, 0.f, 1.f));
//    m_pGameInstance->Add_DebugRender(vCenter, ViewMatrix.look, true, _vec4(0.f, 0.f, 1.f, 1.f));
//}

void CShadow::Compute_CircumscribedSphere(_Out_ _vec3& vCenter, _Out_ _float& fRadius, const array<_vec3, 8>& FrustumPoints, _vec4 vColor)
{
    _vec3 Triangle[3] {};
    Triangle[0] = FrustumPoints[0];
    Triangle[1] = FrustumPoints[2];
    Triangle[2] = FrustumPoints[4];

    _vec3 TriangleDir[3] {};
    TriangleDir[0] = Triangle[1] - Triangle[0];
    TriangleDir[1] = Triangle[2] - Triangle[1];
    TriangleDir[2] = Triangle[0] - Triangle[2];

    _vec3 vCrossVector = _vec3::Cross(TriangleDir[0], -1.f * TriangleDir[2]);
    /* (a*b*c) / Area */
    fRadius = TriangleDir[0].Length() * TriangleDir[1].Length() * TriangleDir[2].Length() / (vCrossVector.Length() * 2.f);

    // BC 이등분 점
    _vec3 vHalfPoint = TriangleDir[1] * 0.5f + Triangle[1];
    _float fHalfLength = TriangleDir[1].Length() * 0.5f;
    _float fDist = sqrt(fRadius * fRadius - fHalfLength * fHalfLength);
    _vec3 vToCenterDir = _vec3::Cross(vCrossVector, TriangleDir[1]).Normalized();

    vCenter = vHalfPoint + vToCenterDir * -1.f * fDist;


#ifdef _DEBUG
    const _mat LightWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LightViewMatrices[0]));

    _vec3 vCuboid[8] {};
    vCuboid[0] = vCenter - fRadius * LightWorldMatrix.right + fRadius * LightWorldMatrix.up - fRadius * LightWorldMatrix.look;
    vCuboid[1] = vCenter + fRadius * LightWorldMatrix.right + fRadius * LightWorldMatrix.up - fRadius * LightWorldMatrix.look;
    vCuboid[2] = vCenter + fRadius * LightWorldMatrix.right - fRadius * LightWorldMatrix.up - fRadius * LightWorldMatrix.look;
    vCuboid[3] = vCenter - fRadius * LightWorldMatrix.right - fRadius * LightWorldMatrix.up - fRadius * LightWorldMatrix.look;
    vCuboid[4] = vCenter - fRadius * LightWorldMatrix.right + fRadius * LightWorldMatrix.up + fRadius * LightWorldMatrix.look;
    vCuboid[5] = vCenter + fRadius * LightWorldMatrix.right + fRadius * LightWorldMatrix.up + fRadius * LightWorldMatrix.look;
    vCuboid[6] = vCenter + fRadius * LightWorldMatrix.right - fRadius * LightWorldMatrix.up + fRadius * LightWorldMatrix.look;
    vCuboid[7] = vCenter - fRadius * LightWorldMatrix.right - fRadius * LightWorldMatrix.up + fRadius * LightWorldMatrix.look;

    for (_uint i = 0; i < 4; ++i)
    {
        _vec3 Quad[4] =
        {
            _vec3(vCuboid[(i + 0) % 4]),
            _vec3(vCuboid[(i + 1) % 4]),
            _vec3(vCuboid[(i + 1) % 4 + 4]),
            _vec3(vCuboid[(i + 0) % 4 + 4]),
        };
        m_pGameInstance->Add_DebugRender_Quad(Quad, vColor);
    }
    for (_uint i = 0; i < 2; ++i)
    {
        _vec3 Quad[4] =
        {
            _vec3(vCuboid[4 * i + 0]),
            _vec3(vCuboid[4 * i + 1]),
            _vec3(vCuboid[4 * i + 2]),
            _vec3(vCuboid[4 * i + 3]),
        };
        m_pGameInstance->Add_DebugRender_Quad(Quad, vColor);
    }

    m_pGameInstance->Add_DebugRender_Ray(vCenter, LightWorldMatrix.right, true, _vec4(1.f, 0.f, 0.f, 1.f));
    m_pGameInstance->Add_DebugRender_Ray(vCenter, LightWorldMatrix.up, true, _vec4(0.f, 1.f, 0.f, 1.f));
    m_pGameInstance->Add_DebugRender_Ray(vCenter, LightWorldMatrix.look, true, _vec4(0.f, 0.f, 1.f, 1.f));
#endif
}

#ifdef _DEBUG
array<_vec3, 8> CShadow::Create_TestFrustum() const
{
    array<_vec3, 8> FrustumPoints {};
    FrustumPoints[0] = _vec3(-1.f, 1.f, 0.f);		// left top near
    FrustumPoints[1] = _vec3(1.f, 1.f, 0.f);		// right top near
    FrustumPoints[2] = _vec3(1.f, -1.f, 0.f);		// right bottom near
    FrustumPoints[3] = _vec3(-1.f, -1.f, 0.f);	// left bottom near

    FrustumPoints[4] = _vec3(-1.f, 1.f, 1.f);		// left top far
    FrustumPoints[5] = _vec3(1.f, 1.f, 1.f);		// right top far
    FrustumPoints[6] = _vec3(1.f, -1.f, 1.f);		// right bottom far
    FrustumPoints[7] = _vec3(-1.f, -1.f, 1.f);	// left bottom far

    static _float fRotationX = 0.f;
    static _float fRotationY = 0.f;
    static _float fX = 0.f;
    static _float fY = 0.f;
    static _float fZ = 0.f;

    _mat ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::PROJ);

    for (_uint i = 0; i < 8; ++i)
    {    
        FrustumPoints[i] = FrustumPoints[i].TransformCoord(ProjMatrixInverse);
    }
    for (_uint i = 0; i < 8; ++i)
    {
        FrustumPoints[i] = FrustumPoints[i].TransformCoord(XMMatrixRotationRollPitchYaw(fRotationX, fRotationY, 0.f) * XMMatrixTranslation(fX, fY, fZ));
    }

    if (m_pGameInstance->Key_Press(KEY::UP))
        fRotationX -= 0.02f;
    if (m_pGameInstance->Key_Press(KEY::DOWN))
        fRotationX += 0.02f;
    if (m_pGameInstance->Key_Press(KEY::RIGHT))
        fRotationY += 0.02f;
    if (m_pGameInstance->Key_Press(KEY::LEFT))
        fRotationY -= 0.02f;

    if (m_pGameInstance->Key_Press(KEY::T))
        fZ += 0.2f;
    if (m_pGameInstance->Key_Press(KEY::G))
        fZ -= 0.2f;
    if (m_pGameInstance->Key_Press(KEY::R))
        fY -= 0.2f;
    if (m_pGameInstance->Key_Press(KEY::Y))
        fY += 0.2f;
    if (m_pGameInstance->Key_Press(KEY::F))
        fX -= 0.2f;
    if (m_pGameInstance->Key_Press(KEY::H))
        fX += 0.2f;

    return FrustumPoints;
}

void CShadow::Save_ShadowDebugFrustum()
{
    array<_float4, 8> WorldFrustum = m_pGameInstance->Get_FrustumWorldPoints();

    for (_uint i = 0; i < 8; ++i)
    {
        m_ShadowDebugFrustum[i] = _vec4(WorldFrustum[i]);
    }
}
#endif // _DEBUG

CShadow* CShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CShadow* pInstance = new CShadow(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Create : CShadow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShadow::Free()
{
    super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
