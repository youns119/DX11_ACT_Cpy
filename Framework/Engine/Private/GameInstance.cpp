#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"

#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Renderer.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "RenderTarget_Manager.h"
#include "PixelPicking.h"
#include "Shadow.h"
#include "Frustum.h"

#include "Collision_Manager.h"
#include "Sound_Manager.h"
#include "Physics_Manager.h"
#include "Effect_Manager.h"
#include "CameraManager.h"
#if defined _DEBUG && defined PROFILING
#include "Profiler.h"
#endif // _DEBUG


#include "DebugRender.h"

//#include "Layer.h"
#include "GameObject.h"
//#include "ContainerObject.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	m_iViewportWidth = EngineDesc.iViewportWidth;
	m_iViewportHeight = EngineDesc.iViewportHeight;

	m_hWnd = EngineDesc.hWnd;
	m_hInstance = EngineDesc.hInstance;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pRenderTarget_Manager = CRenderTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext); // use RenderTarget_Manager
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pPixelPicking = CPixelPicking::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPixelPicking)
		return E_FAIL;

	m_pShadow = CShadow::Create(*ppDevice, *ppContext);
	if (nullptr == m_pShadow)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pPhysics_Manager = CPhysics_Manager::Create();
	if (nullptr == m_pPhysics_Manager)
		return E_FAIL;

	m_pEffect_Manager = CEffect_Manager::Create();
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pCamera_Manager = CCameraManager::Create();
	if (m_pCamera_Manager == nullptr)
		return E_FAIL;

#ifdef _DEBUG
	m_pProfiler = CProfiler::Create();
	if (nullptr == m_pProfiler)
		return E_FAIL;
#endif // _DEBUG


	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
#if defined(_DEBUG) && defined(PROFILING)
	m_pProfiler->Count_Frame();
#endif

	_bool isOpenedLevel = false;

	if (nullptr == m_pLevel_Manager)
		return;
	/* 내 게임내에 필요한  다수 객체의 갱신작업을 모두 모아서 수행을 할거다. */

	{
		m_pInput_Device->Update();
		m_pPixelPicking->Update();
	}

	{
		m_pLevel_Manager->Update(fTimeDelta);
	}

	{
		m_pObject_Manager->Priority_Update(fTimeDelta);
		m_pCamera_Manager->Priority_Update(fTimeDelta);
		m_pEffect_Manager->Priority_Update(fTimeDelta);

		m_pPipeLine->Update();
	}


	{
		m_pFrustum->Update();
		m_pShadow->Update();
	}

	{
		m_pObject_Manager->Update(fTimeDelta);
		m_pEffect_Manager->Update(fTimeDelta);
	}
	{
		m_pObject_Manager->Late_Update(fTimeDelta);
		m_pEffect_Manager->Late_Update(fTimeDelta);
	}

	{
		m_pCollision_Manager->Update();
	}
	{
		m_pPhysics_Manager->Update(fTimeDelta);
	}

	{
		m_pSound_Manager->Update(fTimeDelta);
	}

	{
		m_pObject_Manager->Final_Update(fTimeDelta);
	}
}

HRESULT CGameInstance::Render_Begin(const _float4& vClearColor)
{
	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	if (FAILED(m_pRenderer->Render()))
		return E_FAIL;

	/*return*/ m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::Render_End()
{
	return m_pGraphic_Device->Present();
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	m_pPrototype_Manager->Clear(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);

}

void CGameInstance::Clear_Prototype(_uint iLevelIndex)
{
	m_pPrototype_Manager->Clear(iLevelIndex);
}

void CGameInstance::Clear_Object(_uint iLevelIndex)
{
	m_pObject_Manager->Clear(iLevelIndex);
}


#pragma region RAY_CASTING
_bool CGameInstance::Check_IsAppFocused() const
{
	HWND foregroundWindow = GetForegroundWindow();
	HWND activeWindow = GetActiveWindow();

	// 현재 윈도우가 활성화된 상태인지 확인
	if (foregroundWindow != m_hWnd || activeWindow != m_hWnd)
		return FALSE;
	else
		return TRUE;
}

_float2 CGameInstance::Calulate_ScreenMousePos()
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	_float2 vMousePos {};
	vMousePos.x = (_float)ptMouse.x;
	vMousePos.y = (_float)ptMouse.y;

	return vMousePos;
}

_vector CGameInstance::Calculate_ScreenToWorldRay(const _float2& vMousePos) const
{
	_float4 vClipPos {};
	vClipPos.x = (vMousePos.x / m_iViewportWidth) * 2.f - 1.f;
	vClipPos.y = 1.f - (vMousePos.y / m_iViewportHeight) * 2.f;
	vClipPos.z = 0.f; // 여기에 어떤 쓰레기 값이 들어가도 상관없음, 연산 과정중에 이 값이 영향을 주는 값은 view space 에서의 w 값이다.
	vClipPos.w = 1.f; // view space 에서 z=1.f 라 가정 ( 연산의 편리함 )

	// Z 곱하기
	_float4 vProjPos {}; // z 나누기 이전, 투영행렬 곱한 이후
	vProjPos.x = vClipPos.x * vClipPos.w;
	vProjPos.y = vClipPos.y * vClipPos.w;
	vProjPos.z = vClipPos.z * vClipPos.w;
	vProjPos.w = vClipPos.w * vClipPos.w; // always 1.f


	// 투영 역행렬 곱하기
	_float4 vViewPos {}; // 뷰 스페이스
	_matrix ProjInverseMatrix = Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::PROJ);
	XMStoreFloat4(&vViewPos, XMVector4Transform(XMLoadFloat4(&vProjPos), ProjInverseMatrix));

	vViewPos.w = 1.f; // 이 값이 무조건 1.f 로 만드는 vClipPos.z 값을 넣어주지 않았다. 그래서 직접 초기화 한다.
	// 혹은 float3 로 연산해도 상관 없을듯


	// 뷰 역행렬 곱하기
	_float4 vWorldPos {};
	_matrix ViewInverseMatirx = Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW);
	XMStoreFloat4(&vWorldPos, XMVector4Transform(XMLoadFloat4(&vViewPos), ViewInverseMatirx));

	_vector CamPos = XMLoadFloat4(&Get_CamPosition());

	return XMLoadFloat4(&vWorldPos) - CamPos;

}

_bool CGameInstance::Ray_Cast_To_Mesh_In_Layer(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist, const _wstring& LayerTag)
{

	//HWND foregroundWindow = GetForegroundWindow();
	//HWND activeWindow = GetActiveWindow();

	//// 현재 윈도우가 활성화된 상태인지 확인
	//if (foregroundWindow != m_hWnd || activeWindow != m_hWnd)
	//	return FALSE;

	const list<CGameObject*>& GameObjects = m_pObject_Manager->Get_GameObjects_In_Layer(Get_CurrentLevelIndex(), LayerTag);

	vector<_float> Distances;

	for (auto& pGameObject : GameObjects)
	{
		_float fTempDist { 0.f };
		if (Ray_Cast_To_Mesh_In_GameObject(vRayOrigin, vRayDirection, fTempDist, pGameObject))
			Distances.push_back(fTempDist);
	}

	if (Distances.empty())
		return false;

	fDist = *std::min_element(Distances.begin(), Distances.end());

	return true;
}

_bool CGameInstance::Ray_Cast_To_Mesh_In_GameObject(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist, CGameObject* pGameObject)
{
	vector<_float> Distances;

	_matrix InversedWorldMatrix = static_cast<CTransform*>(pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Inversed();

	_float fIntersecteDist = 0.f;

	_vector vTransformedRayOrigin = XMVector3TransformCoord(vRayOrigin, InversedWorldMatrix);
	_vector vTransformedRayDirection = XMVector3Normalize(XMVector3TransformNormal(vRayDirection, InversedWorldMatrix));

	CVIBuffer* pVIBuffer = nullptr;
	pVIBuffer = static_cast<CVIBuffer*>(pGameObject->Find_Component(L"Com_VIBuffer"));
	if (nullptr != pVIBuffer)
	{
		if (pVIBuffer->Ray_Cast(vTransformedRayOrigin, vTransformedRayDirection, fIntersecteDist))
			Distances.push_back(fIntersecteDist);
	}

	CModel* pModel = nullptr;
	pModel = static_cast<CModel*>(pGameObject->Find_Component(L"Com_Model"));
	if (nullptr != pModel)
	{
		if (pModel->Ray_Cast(vTransformedRayOrigin, vTransformedRayDirection, fIntersecteDist))
			Distances.push_back(fIntersecteDist);
	}

	_float fPartObjectDist;
	for (auto& Pair : pGameObject->Get_PartObjects())
	{
		if (Ray_Cast_To_Mesh_In_GameObject(vRayOrigin, vRayDirection, fPartObjectDist, Pair.second))
			Distances.push_back(fPartObjectDist);
	}


	if (Distances.empty())
		return FALSE;

	fDist = *std::min_element(Distances.begin(), Distances.end());
	return TRUE;
}

_bool CGameInstance::Ray_Cast_To_Collider_In_Layer(_fvector vRayOrigin, _fvector vRayDirection, _float fRayLength, _Out_ CGameObject*& pHit, _Out_ _float& fDist, const _wstring& LayerTag)
{
	const list<CGameObject*>& GameObjects = m_pObject_Manager->Get_GameObjects_In_Layer(Get_CurrentLevelIndex(), LayerTag);

	vector<pair<_float, CGameObject*>> Hits;

	for (auto& pGameObject : GameObjects)
	{
		auto TempHits = pGameObject->Ray_Cast_To_Collider(vRayOrigin, vRayDirection, fRayLength);
		Hits.insert(Hits.end(), TempHits.begin(), TempHits.end());
	}

	if (Hits.empty())
	{
		pHit = nullptr;
		fDist = 0.f;
		return false;
	}

	pair<_float, CGameObject*> HitObject = *std::min_element(Hits.begin(), Hits.end(),
		[](const pair<_float, CGameObject*>& Src, const pair<_float, CGameObject*> Dst)->_bool
		{
			return Src.first < Dst.first;
		});

	pHit = HitObject.second;
	fDist = HitObject.first;
	return true;

}
_bool CGameInstance::Ray_Cast_To_Collider_In_ColliderGroup(_fvector vRayOrigin, _fvector vRayDirection, _float fRayLength, _Out_ CGameObject*& pHit, _Out_ _float& fDist, _uint iColliderGroup)
{
	return m_pCollision_Manager->Ray_Cast_To_Collider_In_ColliderGroup(
		vRayOrigin, vRayDirection, fRayLength,
		pHit, fDist, iColliderGroup);
}
#pragma endregion


void CGameInstance::Bind_DefaultRenderTarget()
{
	m_pGraphic_Device->Bind_DefaultRenderTarget();
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();;
}

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}
#pragma endregion


#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Open_Level(_uint iNewLevelIndex, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Open_Level(iNewLevelIndex, pNewLevel);
}
_uint CGameInstance::Get_CurrentLevelIndex()
{
	return m_pLevel_Manager->Get_CurrentLevelIndex();
}
#pragma endregion


#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CCloneable* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CCloneable* CGameInstance::Clone_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(iLevelIndex, strPrototypeTag, pArg);
}
vector<_wstring> CGameInstance::Get_PrototypeTags(_uint iLevelIndex)
{
	return m_pPrototype_Manager->Get_PrototypeTags(iLevelIndex);
}
#pragma endregion


#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject_To_Layer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject_To_Layer(iPrototypeLevelIndex, strPrototypeTag, iLevelIndex, strLayerTag, pArg);
}
const std::list<CGameObject*>& CGameInstance::Get_GameObjects_In_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_GameObjects_In_Layer(iLevelIndex, strLayerTag);
}
vector<_wstring> CGameInstance::Get_LayerTags(_uint iLevelIndex) const
{
	return m_pObject_Manager->Get_LayerTags(iLevelIndex);
}
CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}
CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strPartTag, strComponentTag, iIndex);
}
CComponent* CGameInstance::Find_Component_In_PartObject(_uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& strPartTags, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component_In_PartObject(iLevelIndex, strLayerTag, strPartTags, strComponentTag, iIndex);
}
void CGameInstance::Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Delete_GameObject(iLevelIndex, strLayerTag, iIndex);
}
void CGameInstance::Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	return m_pObject_Manager->Delete_GameObject(iLevelIndex, strLayerTag, pGameObject);
}
void CGameInstance::Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& strPartTags, _uint iIndex)
{
	return m_pObject_Manager->Delete_GameObject(iLevelIndex, strLayerTag, strPartTags, iIndex);
}
CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Find_GameObject(iLevelIndex, strLayerTag, iIndex);
}
CGameObject* CGameInstance::Find_GameObject_With_ID(_uint iLevelIndex, const _uint iID)
{
	return m_pObject_Manager->Find_GameObject_With_ID(iLevelIndex, iID);
}
CGameObject* CGameInstance::Find_GameObject_With_Name(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strName)
{
	return m_pObject_Manager->Find_GameObject_With_Name(iLevelIndex, strLayerTag, strName);
}
HRESULT CGameInstance::Add_PartObject(const _wstring& strPartTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& PartObjectTags, _uint iIndex, void* pArg)
{
	CGameObject* pRootGameObejct = m_pObject_Manager->Find_GameObject(iLevelIndex, strLayerTag, iIndex);
	CGameObject* pParentObject = pRootGameObejct->Find_PartObject(PartObjectTags);
	return pParentObject->Add_PartObject(strPartTag, iPrototypeLevelIndex, strPrototypeTag, pArg);
}
HRESULT CGameInstance::Add_PartObject(CGameObject* pGameObject, const _wstring& strPartTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return pGameObject->Add_PartObject(strPartTag, iPrototypeLevelIndex, strPrototypeTag, pArg);
}
HRESULT CGameInstance::Add_Layer_To_Lag(const _wstring& LayerTag)
{
	return m_pObject_Manager->Add_Layer_To_Lag(LayerTag);
}
void CGameInstance::Call_Lag(_float fLagRatio, _float fLagTime, _bool bLoop)
{
	m_pObject_Manager->Call_Lag(fLagRatio, fLagTime, bLoop);
	m_pEffect_Manager->Call_Lag(fLagRatio, fLagTime);
}
void CGameInstance::Reset_Lag()
{
	m_pObject_Manager->Reset_Lag();
}
_bool CGameInstance::Get_IsLag() const
{
	return m_pObject_Manager->Get_IsLag();
}
_float CGameInstance::Get_LagRatio() const
{
	return m_pObject_Manager->Get_LagRatio();
}
_float CGameInstance::Get_LagTime() const
{
	return m_pObject_Manager->Get_LagTime();
}
#pragma endregion

#pragma region RENDERER
HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eGroup, CGameObject* pGameObject)
{
	return m_pRenderer->Add_RenderGroup(eGroup, pGameObject);
}
#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP eGroup, CComponent* pDebugCom)
{
	return m_pRenderer->Add_DebugRenderComponent(eGroup, pDebugCom);
}
void CGameInstance::Add_DebugRender_Sphere(DirectX::BoundingSphere Sphere, _float4 vColor)
{
	m_pRenderer->Add_DebugRender_Sphere(Sphere, vColor);
}
void CGameInstance::Add_DebugRender_Sphere(_vec3 vCenter, _float fRadius, _vec4 vColor)
{
	BoundingSphere Sphere;
	Sphere.Center = vCenter._float3();
	Sphere.Radius = fRadius;

	m_pRenderer->Add_DebugRender_Sphere(Sphere, vColor._float4());
}
void CGameInstance::Add_DebugRender_Triangle(_float3 Points[3], _float4 vColor)
{
	m_pRenderer->Add_DebugRender_Triangle(Points, vColor);
}
void CGameInstance::Add_DebugRender_Quad(_vec3 Points[4], _vec4 vColor)
{
	m_pRenderer->Add_DebugRender_Quad(Points, vColor);
}
void CGameInstance::Add_DebugRender_Box(_mat WorldMatrix, _vec4 vColor)
{
	m_pRenderer->Add_DebugRender_Box(WorldMatrix, vColor);
}
void CGameInstance::Add_DebugRender_Ray(_vec3 vOrigin, _vec3 vDir, _bool isNormalize, _vec4 vColor)
{
	m_pRenderer->Add_DebugRender_Ray(vOrigin, vDir, isNormalize, vColor);
}
void CGameInstance::Add_DebugRender_Grid(_vec3 vOrigin, _vec3 vAxisX, _vec3 vAxisY, _float fUnitX, _float fUnitY, _vec4 vColor)
{
	m_pRenderer->Add_DebugRender_Grid(vOrigin, vAxisX, vAxisY, fUnitX, fUnitY, vColor);
}
void CGameInstance::Add_DebugRender_Capsule(_vec3 vPosition, _vec3 vDirection, _float fRadius, _float fHeight, _float4 vColor)
{
	m_pRenderer->Add_DebugRender_Capsule(vPosition, vDirection, fRadius, fHeight, vColor);
}
void CGameInstance::Add_DebugRender_Function(function<HRESULT()> RenderFunction)
{
	m_pRenderer->Add_DebugRender_Function(RenderFunction);
}
#endif
void CGameInstance::Set_FogParams(const FOG_PARAMS& FogParams)
{
	m_pRenderer->Set_FogParams(FogParams);
}
void CGameInstance::Set_BloomParams(const BLOOM_PARAMS& BloomParams)
{
	m_pRenderer->Set_BloomParams(BloomParams);
}
void CGameInstance::Set_ColorCorrectionParams(const COLORCORRECTION_PARAMS& ToneMappingParams)
{
	m_pRenderer->Set_ColorCorrectionParams(ToneMappingParams);
}
void CGameInstance::Set_SSAOParams(const SSAO_PARAMS& SSAOParams)
{
	m_pRenderer->Set_SSAOParams(SSAOParams);
}
void CGameInstance::Set_RadialBlurParams(const RADIALBLUR_PARAMS& RadialBlurParams)
{
	m_pRenderer->Set_RadialBlurParams(RadialBlurParams);
}
void CGameInstance::Set_ShadowMaskParams(const SHADOWMASK_PARAMS& ShadowMaskParams)
{
	m_pRenderer->Set_ShadowMaskParams(ShadowMaskParams);
}
void CGameInstance::Set_BilateralBlurParams(const BILATERALBLUR_PARAMS& BilateralBlurParams)
{
	m_pRenderer->Set_BilateralBlurParams(BilateralBlurParams);
}
const FOG_PARAMS& CGameInstance::Get_FogParams() const
{
	return m_pRenderer->Get_FogParams();
}
const BLOOM_PARAMS& CGameInstance::Get_BloomParams() const
{
	return m_pRenderer->Get_BloomParams();
}
const COLORCORRECTION_PARAMS& CGameInstance::Get_ColorCorrectionParams() const
{
	return m_pRenderer->Get_ColorCorrectionParams();
}
const SSAO_PARAMS& CGameInstance::Get_SSAOParams() const
{
	return m_pRenderer->Get_SSAOParams();
}
const RADIALBLUR_PARAMS& CGameInstance::Get_RadialBlurParams() const
{
	return m_pRenderer->Get_RadialBlurParams();
}
const SHADOWMASK_PARAMS& CGameInstance::Get_ShadowMaskParams() const
{
	return m_pRenderer->Get_ShadowMaskParams();
}
const BILATERALBLUR_PARAMS& CGameInstance::Get_BilateralBlurParams() const
{
	return m_pRenderer->Get_BilateralBlurParams();
}
void CGameInstance::Set_UIBlur(_bool isRenderBlur)
{
	m_pRenderer->Set_UIBlur(isRenderBlur);
}
void CGameInstance::Skip_SSAO(_bool isSkip)
{
	m_pRenderer->Skip_SSAO(isSkip);
}
void CGameInstance::Skip_Bloom(_bool isSkip)
{
	m_pRenderer->Skip_Bloom(isSkip);
}
void CGameInstance::Set_RenderOption(CRenderer::RENDER_OPTION Option)
{
	m_pRenderer->Set_RenderOption(Option);
}
_bool CGameInstance::Is_SkipSSAO() const
{
	return m_pRenderer->Is_SkipSSAO();
}
_bool CGameInstance::Is_SkipBloom() const
{
	return m_pRenderer->Is_SkipBloom();
}
const CRenderer::RENDER_OPTION& CGameInstance::Get_RenderOption() const
{
	return m_pRenderer->Get_RenderOption();
}
#pragma endregion


#pragma region PIPELINE
_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORM eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORM eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}
_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}
_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM eState) const
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}
void CGameInstance::Set_Transform(CPipeLine::TRANSFORM eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}
const _float4& CGameInstance::Get_CamPosition() const
{
	return m_pPipeLine->Get_CamPosition();
}
#pragma endregion


#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}
_bool CGameInstance::Key_Down(KEY eKey) const
{
	return m_pInput_Device->Key_Down(eKey);
}
_bool CGameInstance::Key_Press(KEY eKey) const
{
	return m_pInput_Device->Key_Press(eKey);
}
_bool CGameInstance::Key_Up(KEY eKey) const
{
	return m_pInput_Device->Key_Up(eKey);
}
_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouse) const
{
	return m_pInput_Device->Mouse_Down(eMouse);
}
_bool CGameInstance::Mouse_Press(MOUSEKEYSTATE eMouse) const
{
	return m_pInput_Device->Mouse_Press(eMouse);
}
_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouse) const
{
	return m_pInput_Device->Mouse_Up(eMouse);
}
#pragma endregion


#pragma region LIGHT_MANAGER
const LIGHT_DESC& CGameInstance::Get_LightDesc(const _wstring& strLightTag) const
{
	return m_pLight_Manager->Get_LightDesc(strLightTag);
}
void CGameInstance::Set_LightDesc(const _wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
	m_pLight_Manager->Set_LightDesc(strLightTag, LightDesc);
}
HRESULT CGameInstance::Add_Light(const _wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(strLightTag, LightDesc);
}
void CGameInstance::Remove_Light(const _wstring& strLightTag)
{
	m_pLight_Manager->Remove_Light(strLightTag);
}
HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render_Lights(pShader, pVIBuffer);
}
void CGameInstance::Toggle_Light(const _wstring& strLightTag)
{
	m_pLight_Manager->Toggle_Light(strLightTag);
}
void CGameInstance::Turn_On_Light(const _wstring& strLightTag)
{
	m_pLight_Manager->Turn_On(strLightTag);
}
void CGameInstance::Turn_Off_Light(const _wstring& strLightTag)// 1al meokneundae 3al ddulgeum
{
	m_pLight_Manager->Turn_Off(strLightTag);
}
void CGameInstance::Save_Light(HANDLE hFile, LIGHT_TYPE eType)
{
	m_pLight_Manager->Save_Light(hFile, eType);
}
void CGameInstance::Clear_Lights()
{
	m_pLight_Manager->Clear_Lights();
}
_float4 CGameInstance::Set_RotationY(_float4 vDir, _float fAngle)
{
	return m_pLight_Manager->Set_LightRotateY(vDir, fAngle);
}
#pragma endregion


#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}
HRESULT CGameInstance::Render_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, const _float fScale, SORTING eSorting, SpriteEffects effects, _float layerDepth)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, fRotation, vOrigin, fScale, eSorting, effects, layerDepth);
}
HRESULT CGameInstance::Render_Font_In_World(const _wstring& strFontTag, const _tchar* pText, const _float4x4& WorldMatrix, const _float2& vPosition, _fvector vColor, const _float2& vOrigin, const _float fScale, SORTING eSorting)
{
	return m_pFont_Manager->Render_In_World(strFontTag, pText, WorldMatrix, vPosition, vColor, vOrigin, fScale, eSorting);
}
HRESULT CGameInstance::Render_Font_In_Screen(const _wstring& strFontTag, const _tchar* pText, const _float4x4& WorldMatrix, const _float4x4& ViewMatrix, const _float4x4& ProjMatrix, const _float2& vPosition, _fvector vColor, const _float2& vOrigin, const _float fScale, SORTING eSorting)
{
	return m_pFont_Manager->Render_In_Screen(strFontTag, pText, WorldMatrix, ViewMatrix, ProjMatrix, vPosition, vColor, vOrigin, fScale, eSorting);
}
_vector CGameInstance::Measure_FontString(const _wstring& strFontTag, const _tchar* pText, _bool bIgnoreWhitespce)
{
	return m_pFont_Manager->Measure_FontString(strFontTag, pText, bIgnoreWhitespce);
}
#pragma endregion


#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize, _uint iMipLevel)
{
	return m_pRenderTarget_Manager->Add_RenderTarget(strRenderTargetTag, iWidth, iHeight, ePixelFormat, vClearColor, iArraySize, iMipLevel);
}

HRESULT CGameInstance::Bind_RT_To_Shader(const _wstring& strRenderTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pRenderTarget_Manager->Bind_RT_To_Shader(strRenderTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag)
{
	return m_pRenderTarget_Manager->Add_MRT(strMRTTag, strRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, _bool isClear, ID3D11DepthStencilView* pDSV)
{
	return m_pRenderTarget_Manager->Begin_MRT(strMRTTag, isClear, pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pRenderTarget_Manager->End_MRT();
}

void CGameInstance::Copy_RT_Resource(const _wstring& strRenderTargetTag, ID3D11Texture2D* pTexture2D)
{
	m_pRenderTarget_Manager->Copy_RT_Resource(strRenderTargetTag, pTexture2D);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pRenderTarget_Manager->Ready_RT_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_MRT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pRenderTarget_Manager->Render_MRT_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif //_DEBUG

#pragma endregion

#pragma region PixelPicking
_bool CGameInstance::Compute_PixelPickPos(_float3& Out)
{
	return m_pPixelPicking->Compute_PickPos(Out);
}
_vector CGameInstance::Compute_WorldPos(const _float2& vViewportPos)
{
	return m_pPixelPicking->Compute_WorldPos(vViewportPos);
}
_bool CGameInstance::Compute_ObjectID(_uint& Out)
{
	return m_pPixelPicking->Compute_ObjectID(Out);
}
_bool CGameInstance::Compute_ObjectID_WorldPos(const _float2& vViewportPos, _uint& Out)
{
	return m_pPixelPicking->Compute_ObjectID_WorldPos(vViewportPos, Out);
}
#pragma endregion


#pragma region Shadow
HRESULT CGameInstance::SetUp_ShadowLight(_fvector vEye, _fvector vAt, _float fLightAngle, _float fAspect, _float fNear, _float fFar)
{
	return m_pShadow->SetUp_ShadowLight(vEye, vAt, fLightAngle, fAspect, fNear, fFar);
}
HRESULT CGameInstance::SetUp_ShadowLight(_vec3 vLightDir, _float fNear, _float fFar)
{
	return m_pShadow->SetUp_ShadowLight(vLightDir, fNear, fFar);
}
HRESULT CGameInstance::Bind_ShadowMatrices_To_Shader(CShader* pShader, const _char* pViewConstantName, const _char* pProjConstantName) const
{
	return m_pShadow->Bind_Matrices(pShader, pViewConstantName, pProjConstantName);
}
HRESULT CGameInstance::Bind_CascadeCircumSphere_To_Shader(CShader* pShader, const _char* pCircueSphereConstantName) const
{
	return m_pShadow->Bind_CascadeCircumSphere_To_Shader(pShader, pCircueSphereConstantName);
}
#pragma endregion

#pragma region FRUSTUM
void CGameInstance::Transform_Frustum_LocalSpace(_fmatrix WorldMatrix)
{
	return m_pFrustum->Transform_LocalSpace(WorldMatrix);
}
_bool CGameInstance::Is_InFrustum_WorldSpace(_fvector vWorldPoint, _float fRange)
{
	return m_pFrustum->Is_InWorldSpace(vWorldPoint, fRange);
}

_bool CGameInstance::Is_InFrustum_LocalSpace(_fvector vLocalPoint, _float fRange)
{
	return m_pFrustum->Is_InLocalSpace(vLocalPoint, fRange);
}
const array<_float4, 8>& CGameInstance::Get_FrustumWorldPoints()
{
	return m_pFrustum->Get_WorldPoints();
}
const array<_float4, 8>& CGameInstance::Get_FrustumOriginalPoints()
{
	return m_pFrustum->Get_OriginalPoints();
}
#pragma endregion


#pragma region COLLISION_MANAGER
void CGameInstance::Set_CollisionEnableGroup(_uint iSrcGroup, _uint iDstGroup, _bool isEnable)
{
	m_pCollision_Manager->Set_CollisionEnableGroup(iSrcGroup, iDstGroup, isEnable);
}
void CGameInstance::Add_ColliderGroup(CGameObject* pGameObject, CCollider* pCollider, _uint iGroup)
{
	m_pCollision_Manager->Add_ColliderGroup(pGameObject, pCollider, iGroup);
}
void CGameInstance::Clear_CollisionManager()
{
	m_pCollision_Manager->Clear();
}
#pragma endregion


#pragma region SOUND_MANAGER
void CGameInstance::Play_Sound(const _wstring& strSoundTag, _uint iChannelID, _float fVolume)
{
	m_pSound_Manager->Play_Sound(strSoundTag, iChannelID, fVolume);
}
void CGameInstance::Play_Sound_Loop(const _wstring& strSoundTag, _uint iChannelID, _float fVolume)
{
	m_pSound_Manager->Play_Sound_Loop(strSoundTag, iChannelID, fVolume);
}
void CGameInstance::Stop_Sound(_uint iChannelID)
{
	m_pSound_Manager->Stop_Sound(iChannelID);
}
void CGameInstance::Stop_Sound_All()
{
	m_pSound_Manager->Stop_Sound_All();
}
_bool CGameInstance::IsPlaying(_uint iChannelID)
{
	return m_pSound_Manager->IsPlaying(iChannelID);
}
void CGameInstance::Set_MasterVolume(_float fVolume)
{
	m_pSound_Manager->Set_MasterVolume(fVolume);
}
void CGameInstance::Set_ChannelVolume(_uint iChannelID, _float fVolume)
{
	m_pSound_Manager->Set_ChannelVolume(iChannelID, fVolume);
}
void CGameInstance::Set_RawChannelVolume(_uint iChannelID, _float fVolume)
{
	m_pSound_Manager->Set_RawChannelVolume(iChannelID, fVolume);
}
void CGameInstance::Set_ChannelVolume_With_SpatialDesc(_uint iChannelID, _vec3 vPosition, _vec2 vDistRange, _vec2 vVolumeRange)
{
	m_pSound_Manager->Set_ChannelVolume_With_SpatialDesc(iChannelID, vPosition, vDistRange, vVolumeRange);
}
void CGameInstance::LoadSoundFile(const _string& strPath)
{
	m_pSound_Manager->LoadSoundFile(strPath);
}

void CGameInstance::FadeSwap_Sound(const _wstring& strNewSoundTag, _uint iChannelID, _float fSwapTime, _float fVolume)
{
	m_pSound_Manager->FadeSwap_Sound(strNewSoundTag, iChannelID, fSwapTime, fVolume);
}

void CGameInstance::Set_ListenerPositoin(const _vec3& vPosition)
{
	m_pSound_Manager->Set_ListenerPositoin(vPosition);
}

void CGameInstance::Clear_SoundManager()
{
	m_pSound_Manager->Clear();
}

#pragma endregion


#pragma region PHYSICS_MANAGER
void CGameInstance::Simulate_Physics(CPhysics* pPhysics)
{
	m_pPhysics_Manager->Simulate_Physics(pPhysics);
}
void CGameInstance::Simulate_Physics(CPhysics_Controller* pController)
{
	m_pPhysics_Manager->Simulate_Physics(pController);
}
_bool CGameInstance::Ray_Cast_To_Physics(_vec3 vOrigin, _vec3 vDir, _float fLength, _Out_ _float& fDist, _uint iPhysicsFlags)
{
	return m_pPhysics_Manager->Ray_Cast(vOrigin, vDir, fLength, fDist, iPhysicsFlags);
}
PxController* CGameInstance::Create_PhysicsCharacterController(const CPhysics_Controller::DESC& Desc, CPhysics_Controller::USER_DATA* pUserData, _Out_ PxControllerFilters& outFilters, _Out_ _uint& iChunkIndexX, _Out_ _uint& iChunkIndexZ)
{
	return m_pPhysics_Manager->Create_Controller(Desc, pUserData, outFilters, iChunkIndexX, iChunkIndexZ);
}
void CGameInstance::Add_MovablePhysics_To_Chunk(const _vec3& vPosition, _Out_ _uint& iChunkIndexX, _Out_ _uint& iChunkIndexZ)
{
	m_pPhysics_Manager->Add_Movable_To_Chunk(vPosition, iChunkIndexX, iChunkIndexZ);
}
void CGameInstance::Resist_Physics_To_Chunk(_vec3 vMin, _vec3 vMax, CPhysics* pPhysics)
{
	m_pPhysics_Manager->Resist_Physics_To_Chunk(vMin, vMax, pPhysics);
}
void CGameInstance::Update_ChunkIndex_On_PhysicsMove(_vec3 vMovedPosition, _Inout_ _uint& iChunkIndexX, _Inout_ _uint& iChunkIndexZ)
{
	m_pPhysics_Manager->Update_ChunkIndex_On_Move(vMovedPosition, iChunkIndexX, iChunkIndexZ);
}
HRESULT CGameInstance::Setup_PhysicsScene(const _vec2& vMin, const _vec2& vMax, _float fChunkSize)
{
	return m_pPhysics_Manager->SetUp_Scene(vMin, vMax, fChunkSize);
}
void CGameInstance::Remove_Controller_From_Chunk(_uint iX, _uint iZ)
{
	m_pPhysics_Manager->Remove_Controller_From_Chunk(iX, iZ);
}
void CGameInstance::Clear_PhysicsManager()
{
	m_pPhysics_Manager->Clear();
}
void CGameInstance::Erase_DynamicPhysicsHash(PxActor* pActor)
{
	m_pPhysics_Manager->Erase_DynamicPhysicsHash(pActor);
}

void CGameInstance::Resist_DynamicPhysicsHash(PxActor* pActor, CPhysics* pPhysics)
{
	m_pPhysics_Manager->Resist_DynamicPhysicsHash(pActor, pPhysics);
}

#pragma endregion

#pragma region EFFECT_MANAGER
CEffectObject* CGameInstance::Call_Effect(_uint iEffectType, _fvector vStartPos, _fvector vLookAt, _bool bIsPartObject, const _float4x4* pAdoptedWorldMatrix, _float fLookRotateDegree, _bool bFlip, const _float4x4* pParentWorldMatrix)
{
	return m_pEffect_Manager->Call_Effect(iEffectType, vStartPos, vLookAt, bIsPartObject, pAdoptedWorldMatrix, fLookRotateDegree, bFlip, pParentWorldMatrix);
}
HRESULT CGameInstance::Pool_Effect(_uint iEffectType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iEffectNum, void* pArg)
{
	return m_pEffect_Manager->Pool_Effect(iEffectType, iPrototypeLevelIndex, strPrototypeTag, iEffectNum, pArg);
}
void CGameInstance::Clear_Effects()
{
	m_pEffect_Manager->Clear();
}
#pragma endregion

#pragma region CAMERA_MANAGER

CCamera* CGameInstance::Get_CurrCam()
{
	return m_pCamera_Manager->Get_CurrCam();
}

void CGameInstance::Set_CurrCam(CCamera* pCamera)
{
	m_pCamera_Manager->Set_CurrCam(pCamera);
}

void CGameInstance::Add_Camera(const _wstring& strTag, CCamera* pCamera)
{
	m_pCamera_Manager->Add_Camera(strTag, pCamera);
}
HRESULT CGameInstance::Camera_Transition(const _wstring& strDst, _uint iLerp, _float fLerpTime)
{
	return m_pCamera_Manager->Camera_Transition(strDst, iLerp, fLerpTime);
}
void CGameInstance::Clear_CameraManager()
{
	m_pCamera_Manager->Clear();
}

#pragma endregion

#ifdef _DEBUG

CScopeTimer CGameInstance::Profile_Scope(const _wstring& strTag)
{
	return move(m_pProfiler->Profile_Scope(strTag));
}

_bool CGameInstance::Flush_ProfiledRecord_Output(_uint iAverageInterval)
{
	return m_pProfiler->Flush_Output(iAverageInterval);
}

_bool CGameInstance::Flush_ProfiledRecord(_tchar* pOutBuffer, size_t iBufferLength, _uint iAverageInterval)
{
	return m_pProfiler->Flush(pOutBuffer, iBufferLength, iAverageInterval);
}

_bool CGameInstance::Flush_ProfiledRecord(_Out_ _wstring& strOutBuffer, _uint iAverageInterval)
{
	return m_pProfiler->Flush(strOutBuffer, iAverageInterval);
}

#endif // _DEBUG

void CGameInstance::Release_Engine()
{
	// weak pointer 가 없다보니까 어쩔 수 없이 이런식으로 관리하게 되는거 같다.
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pPixelPicking);
	Safe_Release(m_pShadow);
	Safe_Release(m_pFrustum);

	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPhysics_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pLight_Manager);
#ifdef _DEBUG
	Safe_Release(m_pProfiler);
#endif // _DEBUG

	//CGameInstance::DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();

	// 이러면 이제 free 에서 하는일이 없어진다.
	// 나는 이게 일관성이 없어진다고 생각이 되어 걱정된다.
}
