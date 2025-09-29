#pragma once

#include "Base.h"
#include "PipeLine.h"
#include "Renderer.h"
#include "Prototype_Manager.h"
#include "CameraManager.h"

#ifdef _DEBUG
#include "Profiler.h"
#endif // _DEBUG


/* 0. 엔진에서 클라이언트 프로젝트에 직접 보여주는 클래스 : CGameInstance, 부모클래스(CBase, CLevel, CGameObject, CComponent..) */
/* 1. 클라이언트 프로젝트에서 엔진프로젝트에 있는 기능을 호출하고자한다라면 무조건 GameInstance를 통해서 호출할 수 있도록 하겠다.  */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	/* 클라에서 호출할 만한 엔진의 기능을 위해 필요한 초기화를 수행한다. */
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Render_Begin(const _float4& vClearColor);
	HRESULT Draw();
	HRESULT Render_End();
	void Clear_Prototype(_uint iLevelIndex);
	void Clear_Object(_uint iLevelIndex);
	void Clear(_uint iLevelIndex);

	_uint2 Get_ViewportSize() const { return _uint2(m_iViewportWidth, m_iViewportHeight); }


#pragma region RAY_CASTING
	_bool Check_IsAppFocused() const;
	_float2 Calulate_ScreenMousePos();
	_vector Calculate_ScreenToWorldRay(const _float2& vMousePos) const;
	_bool Ray_Cast_To_Mesh_In_Layer(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist, const _wstring& LayerTag);
	_bool Ray_Cast_To_Mesh_In_GameObject(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist, CGameObject* pGameObject);
	_bool Ray_Cast_To_Collider_In_Layer(_fvector vRayOrigin, _fvector vRayDirection, _float fRayLength, _Out_ CGameObject*& pHit, _Out_ _float& fDist, const _wstring& LayerTag);
	_bool Ray_Cast_To_Collider_In_ColliderGroup(_fvector vRayOrigin, _fvector vRayDirection, _float fRayLength, _Out_ CGameObject*& pHit, _Out_ _float& fDist, _uint iColliderGroup);
#pragma endregion 


#pragma region GRAPHIC_DEVICE
public:
	void Bind_DefaultRenderTarget(); // imgui docking(multi viewport)를 사용하기 위해서 억지로 하나 만들었음
	HRESULT Clear_DepthStencil_View();
#pragma endregion


#pragma region TIMER_MANAGER
public: /* For.Timer_Manager */
	_float			Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT			Add_Timer(const _wstring& strTimerTag);
	void			Compute_TimeDelta(const _wstring& strTimerTag);
#pragma endregion


#pragma region LEVEL_MANAGER
public:
	HRESULT Open_Level(_uint iNewLevelIndex, class CLevel* pNewLevel);
	_uint Get_CurrentLevelIndex();
#pragma endregion


#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CCloneable* pPrototype);
	class CCloneable* Clone_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	vector<_wstring> Get_PrototypeTags(_uint iLevelIndex);
#pragma endregion


#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject_To_Layer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	const std::list<CGameObject*>& Get_GameObjects_In_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	vector<_wstring> Get_LayerTags(_uint iLevelIndex) const;
	CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	// recommand using Find_Component_In_PartObject() instead of this;
	CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex = 0);
	CComponent* Find_Component_In_PartObject(_uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& strPartTags, const _wstring& strComponentTag, _uint iIndex = 0);
	void Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	void Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
	void Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& strPartTags, _uint iIndex = 0);
	CGameObject* Find_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	CGameObject* Find_GameObject_With_ID(_uint iLevelIndex, const _uint iID);
	CGameObject* Find_GameObject_With_Name(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strName);
	//CGameObject* Find_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, const _vector<_wstring>& strPartTags, _uint iIndex = 0);
	HRESULT Add_PartObject(const _wstring& strPartTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& PartObjectTags, _uint iIndex = 0, void* pArg = nullptr);
	HRESULT Add_PartObject(CGameObject* pGameObject, const _wstring& strPartTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);


	/* For Hit Lag */
	HRESULT Add_Layer_To_Lag(const _wstring& LayerTag);
	void Call_Lag(_float fLagRatio, _float fLagTime, _bool bLoop = false);
	void Reset_Lag();

	_float Get_LagRatio() const;
	_float Get_LagTime() const;
	_bool Get_IsLag() const;
#pragma endregion


#pragma region REDNERER
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eGroup, class CGameObject* pGameObject);
#ifdef _DEBUG
	HRESULT Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP eGroup, CComponent* pDebugCom);
	#pragma region DEBUG_RENDER
	void Add_DebugRender_Sphere(DirectX::BoundingSphere Sphere, _float4 vColor);
	void Add_DebugRender_Sphere(_vec3 vCenter, _float fRadius, _vec4 vColor);
	void Add_DebugRender_Triangle(_float3 Points[3], _float4 vColor);
	void Add_DebugRender_Quad(_vec3 Points[4], _vec4 vColor);
	void Add_DebugRender_Box(_mat WoalrMatrix, _vec4 vColor);
	void Add_DebugRender_Ray(_vec3 vOrigin, _vec3 vDir, _bool isNormalize, _vec4 vColor);
	void Add_DebugRender_Grid(_vec3 vOrigin, _vec3 vAxisX, _vec3 vAxisY, _float fUnitX, _float fUnitY, _vec4 vColor);
	void Add_DebugRender_Capsule(_vec3 vPosition, _vec3 vDirection, _float fRadius, _float fHeight, _float4 vColor);
	void Add_DebugRender_Function(function<HRESULT()> RenderFunction);
#pragma endregion
#endif
	void Set_FogParams(const FOG_PARAMS& FogParams);
	void Set_BloomParams(const BLOOM_PARAMS& BloomParams);
	void Set_ColorCorrectionParams(const COLORCORRECTION_PARAMS& ToneMappingParams);
	void Set_SSAOParams(const SSAO_PARAMS& SSAOParams);
	void Set_RadialBlurParams(const RADIALBLUR_PARAMS& SSAOParams);
	void Set_BilateralBlurParams(const BILATERALBLUR_PARAMS& BilateralBlurParams);
	void Set_ShadowMaskParams(const SHADOWMASK_PARAMS& ShadowMaskParams);
	const FOG_PARAMS& Get_FogParams() const;
	const BLOOM_PARAMS& Get_BloomParams() const;
	const COLORCORRECTION_PARAMS& Get_ColorCorrectionParams() const;
	const SSAO_PARAMS& Get_SSAOParams() const;
	const RADIALBLUR_PARAMS& Get_RadialBlurParams() const;
	const BILATERALBLUR_PARAMS& Get_BilateralBlurParams() const;
	const SHADOWMASK_PARAMS& Get_ShadowMaskParams() const;

	void Set_UIBlur(_bool isRenderBlur);
	void Skip_SSAO(_bool isSkip);
	void Skip_Bloom(_bool isSkip);
	void Set_RenderOption(CRenderer::RENDER_OPTION Option);

	_bool Is_SkipSSAO() const;
	_bool Is_SkipBloom() const;
	const CRenderer::RENDER_OPTION& Get_RenderOption() const;
#pragma endregion


#pragma region PIPELINE
	_float4x4 Get_Transform_Float4x4(CPipeLine::TRANSFORM eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORM eState) const;
	_float4x4 Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORM eState) const;
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM eState)const;

	void Set_Transform(CPipeLine::TRANSFORM eState, _fmatrix TransformMatrix);

	const _float4& Get_CamPosition() const;
#pragma endregion


#pragma region INPUT_DEVICE
	_byte Get_DIKeyState(_ubyte byKeyID);
	_byte Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	_bool Key_Down(KEY eKey) const;
	_bool Key_Press(KEY eKey) const;
	_bool Key_Up(KEY eKey) const;

	_bool Mouse_Down(MOUSEKEYSTATE eMouse) const;
	_bool Mouse_Press(MOUSEKEYSTATE eMouse) const;
	_bool Mouse_Up(MOUSEKEYSTATE eMouse) const;
#pragma endregion


#pragma region LIGHT_MANAGER
	const LIGHT_DESC& Get_LightDesc(const _wstring& strLightTag) const;
	void Set_LightDesc(const _wstring& strLightTag, const LIGHT_DESC& LightDesc);
	HRESULT Add_Light(const _wstring& strLightTag, const LIGHT_DESC& LightDesc);
	void Remove_Light(const _wstring& strLightTag);
	HRESULT Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer);
	void Toggle_Light(const _wstring& strLightTag);
	void Turn_On_Light(const _wstring& strLightTag);
	void Turn_Off_Light(const _wstring& strLightTag);
	void Save_Light(HANDLE hFile, LIGHT_TYPE eType);
	void Clear_Lights();
	_float4 Set_RotationY(_float4 vDir, _float fAngle);
#pragma endregion


#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), const _float fScale = 1.f, SORTING eSorting = SORTING::NONE, SpriteEffects effects = SpriteEffects_None, _float layerDepth = 0);
	HRESULT Render_Font_In_World(const _wstring& strFontTag, const _tchar* pText, const _float4x4& WorldMatrix, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		const _float2& vOrigin = _float2(0.f, 0.f), const _float fScale = 1.f, SORTING eSorting = SORTING::NONE);
	HRESULT Render_Font_In_Screen(const _wstring& strFontTag, const _tchar* pText, const _float4x4& WorldMatrix, const _float4x4& ViewMatrix, const _float4x4& ProjMatrix, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		const _float2& vOrigin = _float2(0.f, 0.f), const _float fScale = 1.f, SORTING eSorting = SORTING::NONE);
	_vector Measure_FontString(const _wstring& strFontTag, const _tchar* pText, _bool bIgnoreWhitespce = true);
#pragma endregion


#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize = 1, _uint iMipLevel = 1);
	HRESULT Bind_RT_To_Shader(const _wstring& strRenderTargetTag, CShader* pShader, const _char* pConstantName);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, _bool isClear = true, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	void Copy_RT_Resource(const _wstring& strRenderTargetTag, ID3D11Texture2D* pTexture2D);
#ifdef _DEBUG
public:
	HRESULT Ready_RT_Debug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG
#pragma endregion


#pragma region PixelPicking
	_bool Compute_PixelPickPos(_float3& Out);
	_vector Compute_WorldPos(const _float2& vViewportPos);
	_bool Compute_ObjectID(_uint& Out);
	_bool Compute_ObjectID_WorldPos(const _float2& vViewportPos, _uint& Out);
#pragma endregion


#pragma region Shadow
	HRESULT SetUp_ShadowLight(_fvector vEye, _fvector vAt, _float fLightAngle, _float fAspect, _float fNear, _float fFar);
	HRESULT SetUp_ShadowLight(_vec3 vLightDir, _float fNear = 0.0f, _float fFar = 1000.f);
	HRESULT Bind_ShadowMatrices_To_Shader(class CShader* pShader, const _char* pViewConstantName, const _char* pProjConstantName) const;
	HRESULT Bind_CascadeCircumSphere_To_Shader(CShader* pShader, const _char* pCircueSphereConstantName) const;
#pragma endregion

#pragma region FRUSTUM
	void Transform_Frustum_LocalSpace(_fmatrix WorldMatrix);
	_bool Is_InFrustum_WorldSpace(_fvector vWorldPoint, _float fRange = 0.f);
	_bool Is_InFrustum_LocalSpace(_fvector vLocalPoint, _float fRange = 0.f);
	const array<_float4, 8>& Get_FrustumWorldPoints();
	const array<_float4, 8>& Get_FrustumOriginalPoints();
#pragma endregion

#pragma region COLLISION_MANAGER
	void Set_CollisionEnableGroup(_uint iSrcGroup, _uint iDstGroup, _bool isEnable);
	void Add_ColliderGroup(CGameObject* pGameObject, CCollider* pCollider, _uint iGroup);
	void Clear_CollisionManager();
#pragma endregion


#pragma region SOUND_MANAGER
	void Play_Sound(const _wstring& strSoundTag, _uint iChannelID, _float fVolume);
	void Play_Sound_Loop(const _wstring& strSoundTag, _uint iChannelID, _float fVolume);
	void Stop_Sound(_uint iChannelID);
	void Stop_Sound_All();
	_bool IsPlaying(_uint iChannelID);
	void Set_MasterVolume(_float fVolume);
	void Set_ChannelVolume(_uint iChannelID, _float fVolume);
	void Set_RawChannelVolume(_uint iChannelID, _float fVolume);
	void Set_ChannelVolume_With_SpatialDesc(_uint iChannelID, _vec3 vPosition, _vec2 vDistRange, _vec2 vVolumeRange);
	void LoadSoundFile(const _string& strPath);
	void FadeSwap_Sound(const _wstring& strNewSoundTag, _uint iChannelID, _float fSwapTime, _float fVolume);
	void Set_ListenerPositoin(const _vec3& vPosition);
	void Clear_SoundManager();
#pragma endregion


#pragma region PHYSICS_MANAGER
	void Simulate_Physics(CPhysics* pPhysics);
	void Simulate_Physics(CPhysics_Controller* pController);
	_bool Ray_Cast_To_Physics(_vec3 vOrigin, _vec3 vDir, _float fLength, _Out_ _float& fDist, _uint iPhysicsFlags);
	PxController* Create_PhysicsCharacterController(
		const CPhysics_Controller::DESC& Desc,
		CPhysics_Controller::USER_DATA* pUserData,
		_Out_ PxControllerFilters& outFilters,
		_Out_ _uint& iChunkIndexX,
		_Out_ _uint& iChunkIndexZ);
	void Add_MovablePhysics_To_Chunk(const _vec3& vPosition, _Out_ _uint& iChunkIndexX, _Out_ _uint& iChunkIndexZ);
	void Resist_Physics_To_Chunk(_vec3 vMin, _vec3 vMax, CPhysics* pPhysics);
	void Update_ChunkIndex_On_PhysicsMove(_vec3 vMovePosition, _Inout_ _uint& iChunkIndexX, _Inout_ _uint& iChunkIndexZ);
	HRESULT Setup_PhysicsScene(const _vec2& vMin, const _vec2& vMax, _float fChunkSize);
	void Remove_Controller_From_Chunk(_uint iX, _uint iZ);
	void Clear_PhysicsManager();
	void Erase_DynamicPhysicsHash(PxActor* pActor);
	void Resist_DynamicPhysicsHash(PxActor* pActor, CPhysics* pPhysics);
#pragma endregion

#pragma region EFFECT_MANAGER
	class CEffectObject* Call_Effect(_uint iEffectType, _fvector vStartPos, _fvector vLookAt, _bool bIsPartObject, const _float4x4* pAdoptedWorldMatrix, _float fLookRotateDegree = 0.f, _bool bFlip = false, const _float4x4* pParentWorldMatrix = nullptr);
	HRESULT Pool_Effect(_uint iEffectType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iEffectNum, void* pArg);
	void	Clear_Effects();
#pragma endregion

#pragma region CAMERA_MANAGER
	CCamera* Get_CurrCam();
	void Set_CurrCam(CCamera* pCamera);
	void Add_Camera(const _wstring& strTag, CCamera* pCamera);
	HRESULT Camera_Transition(const _wstring& strDst, _uint iLerp, _float fLerpTime);
	void Clear_CameraManager();
#pragma endregion

#ifdef _DEBUG
#pragma region PROFILER
	CScopeTimer Profile_Scope(const _wstring& strTag);
	_bool Flush_ProfiledRecord_Output(_uint iAverageInterval = 50);
	_bool Flush_ProfiledRecord(_tchar* pOutBuffer, size_t iBufferLength, _uint iAverageInterval = 50);
	_bool Flush_ProfiledRecord(_Out_ _wstring& strOutBuffer, _uint iAverageInterval = 50);
#pragma endregion
#endif // _DEBUG

private:
	_uint		m_iViewportWidth { 0 };
	_uint		m_iViewportHeight { 0 };

	HINSTANCE		m_hInstance {};
	HWND			m_hWnd {};


	class CTimer_Manager*			m_pTimer_Manager { nullptr };
	class CGraphic_Device*			m_pGraphic_Device { nullptr };
	class CLevel_Manager*			m_pLevel_Manager { nullptr };
	class CObject_Manager*			m_pObject_Manager { nullptr };
	class CPrototype_Manager*		m_pPrototype_Manager { nullptr };
	CRenderer*						m_pRenderer { nullptr };
	CPipeLine*						m_pPipeLine { nullptr };
	class CInput_Device*			m_pInput_Device { nullptr };
	class CLight_Manager*			m_pLight_Manager { nullptr };
	class CFont_Manager*			m_pFont_Manager { nullptr };
	class CRenderTarget_Manager*	m_pRenderTarget_Manager { nullptr };
	class CPixelPicking*			m_pPixelPicking { nullptr };
	class CShadow*					m_pShadow { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager { nullptr };
	class CSound_Manager*			m_pSound_Manager { nullptr };
	class CPhysics_Manager*			m_pPhysics_Manager { nullptr };
	class CEffect_Manager*			m_pEffect_Manager{ nullptr };
	class CCameraManager*			m_pCamera_Manager{ nullptr };
	
#ifdef _DEBUG
	CProfiler*				m_pProfiler {nullptr};
#endif // _DEBUG



public:
	void Release_Engine();
	virtual void Free() override;

};

END