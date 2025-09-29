#pragma once

#include "Base.h"
#include "Renderer_Params.h"
#include "ShaderPasses.h"

/* 1.화면에ㅐ 그려져야할 객체들을 모아둔다.  */
/* 1_1. 내가 그리고자하는 순서대로 객체들을 분류하여 모아둔다. */

/* 2.순서대로 모아놓은 객체들을 저장해놓은 순서대로 돌면서 렌더함수를 호출(렌더콜, 드로우콜)해준다. */

BEGIN(Engine)

class CGameInstance;
class CGameObject;
class CComponent;
class CShader;
class CVIBuffer_Rect;
class CRenderPass_SSAO;
class CRenderPass_Bloom;
class CRenderPass_RadialBlur;
class CRenderPass_ShadowMask;

class CRenderer final : public CBase
{
public:
	enum class RENDERGROUP
	{
		PRIORITY,
		SHADOW,
		NONBLEND,
		NONLIGHT,
		BLEND,

		EFFECTBLEND,
		WEIGHTEDBLEND,
		DISTORTION,

		UI,
		UI_ANIM,
		MAX
	};

#ifdef _DEBUG
	enum class DEBUGRENDERGROUP
	{
		NONBLEND, BLEND, MAX
	};
#endif

	struct RENDER_OPTION
	{
		_bool isSkipSSAO { false };
		_bool isSkipBloom { false };
		_bool isHardShadow { false };
	};


private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eGruop, CGameObject* pGameObject);
	HRESULT Render();

	/* PostProcess Params */
	const FOG_PARAMS& Get_FogParams() const { return m_FogParams; }
	const BLOOM_PARAMS& Get_BloomParams() const { return m_BloomParams; }
	const COLORCORRECTION_PARAMS& Get_ColorCorrectionParams() const { return m_ColorCorrectionParams; }
	const SSAO_PARAMS& Get_SSAOParams() const { return m_SSAOParams; }
	const RADIALBLUR_PARAMS& Get_RadialBlurParams() const { return m_RadialBlurParams; }
	const BILATERALBLUR_PARAMS& Get_BilateralBlurParams() const { return m_BilateralBlurParams; }
	const SHADOWMASK_PARAMS& Get_ShadowMaskParams() const { return m_ShadowMaskParams; }

	void Set_FogParams(const FOG_PARAMS& FogParams) { m_FogParams = FogParams; }
	void Set_BloomParams(const BLOOM_PARAMS& BloomParams) { m_BloomParams = BloomParams; }
	void Set_ColorCorrectionParams(const COLORCORRECTION_PARAMS& ColorCorrectionParams) { m_ColorCorrectionParams = ColorCorrectionParams; }
	void Set_SSAOParams(const SSAO_PARAMS& SSAOParams) { m_SSAOParams = SSAOParams; }
	void Set_RadialBlurParams(const RADIALBLUR_PARAMS& RadialBlurParams) { m_RadialBlurParams = RadialBlurParams; }
	void Set_BilateralBlurParams(const BILATERALBLUR_PARAMS& BilateralBlurParams) { m_BilateralBlurParams = BilateralBlurParams; }
	void Set_ShadowMaskParams(const SHADOWMASK_PARAMS& ShadowMaskParams) { m_ShadowMaskParams = ShadowMaskParams; }

	void Set_UIBlur(_bool isRenderBlur) { m_isRenderBlur = isRenderBlur; }
	void Skip_SSAO(_bool isSkip) { m_isSkipSSAO = isSkip; }
	void Skip_Bloom(_bool isSkip) { m_isSkipBloom = isSkip; }
	void Set_RenderOption(const RENDER_OPTION& Option) { m_RenderOption = Option; }

	_bool Is_SkipSSAO() const { return m_isSkipSSAO; }
	_bool Is_SkipBloom() const { return m_isSkipBloom; }
	const RENDER_OPTION& Get_RenderOption() const { return m_RenderOption; }

#ifdef _DEBUG
public:
	HRESULT Add_DebugRenderComponent(DEBUGRENDERGROUP eGroup, CComponent* pDebugCom);

#pragma region DEBUG_RENDER
	void Add_DebugRender_Sphere(DirectX::BoundingSphere Sphere, _float4 vColor);
	void Add_DebugRender_Triangle(_float3 Points[3], _float4 vColor);
	void Add_DebugRender_Quad(_vec3 Points[4], _vec4 vColor);
	void Add_DebugRender_Box(_mat WorldMatrix, _vec4 vColor);
	void Add_DebugRender_Ray(_vec3 vOrigin, _vec3 vDir, _bool isNomalize, _vec4 vColor);
	void Add_DebugRender_Grid(_vec3 vOrigin, _vec3 vAxisX, _vec3 vAxisY, _float fUnitX, _float fUnitY, _vec4 vColor);
	void Add_DebugRender_Capsule(_vec3 vPosition, _vec3 vDirection, _float fRadius, _float fHeight, _float4 vColor);
	void Add_DebugRender_Function(function<HRESULT()> RenderFunction);
#pragma endregion
#endif

private:
	ID3D11Device*				m_pDevice { nullptr };
	ID3D11DeviceContext*		m_pContext { nullptr };
	CGameInstance*				m_pGameInstance { nullptr };

	CShader*			m_pShaderCom { nullptr };
	CShader*			m_pLightingShaderCom { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom { nullptr };

	_float4x4 m_WorldMatrix {};
	_float4x4 m_ViewMatrix {};
	_float4x4 m_ProjMatrix {};

	list<CGameObject*> m_RenderObjects[(_uint)RENDERGROUP::MAX];

	ID3D11DepthStencilView* m_pShadowDSV { nullptr };
	_uint m_iOriginViewportWidth { 0 };
	_uint m_iOriginViewportHeight { 0 };

	const _uint m_iShadowMapWidth { 2048 };
	const _uint m_iShadowMapHeight { 2048 };

	FOG_PARAMS					m_FogParams {};
	BLOOM_PARAMS				m_BloomParams {};
	COLORCORRECTION_PARAMS		m_ColorCorrectionParams {};
	SSAO_PARAMS					m_SSAOParams {};
	RADIALBLUR_PARAMS			m_RadialBlurParams {};
	BILATERALBLUR_PARAMS		m_BilateralBlurParams {};
	SHADOWMASK_PARAMS			m_ShadowMaskParams {};

	CRenderPass_SSAO*			m_pRenderPass_SSAO { nullptr };
	CRenderPass_Bloom*			m_pRenderPass_Bloom { nullptr };
	CRenderPass_RadialBlur*		m_pRenderPass_RadialBlur { nullptr };
	CRenderPass_ShadowMask*		m_pRenderPass_ShadowMask { nullptr };

	_bool					m_isRenderBlur{ false };
	_bool					m_isSkipSSAO { false };
	_bool					m_isSkipBloom { false };
	RENDER_OPTION		m_RenderOption {};

#ifdef _DEBUG
	list<CComponent*> m_DebugComponents[(_uint)DEBUGRENDERGROUP::MAX];

	class CDebugRender* m_pDebugRender { nullptr };
	ID3D11DepthStencilState* m_pDebugDSS { nullptr };

	size_t m_iDebugMRTID { 0 };
	vector<vector<_wstring>> m_DebugMRTTagsByIndex { vector<_wstring>() };
	CShader* m_pDebugShaderCom { nullptr };
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_ShadowMask();
	HRESULT Render_NonBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred(); // diffuse * shade
	HRESULT Render_NonLight(); // Non Blend + no light -> except Deferred
	HRESULT Render_SSAO(_wstring& strMRTTag, _wstring& strFinalTag);
	HRESULT Render_Blend(_wstring& strMRTTag, _wstring& strFinalTag);
	HRESULT Render_Blend_Effect();
	HRESULT Render_Blend_Weighted();
	HRESULT Render_Distortion();
	HRESULT Render_Effect_Final(_wstring& strFinalTag);
	HRESULT Render_PostProcess_EdgeDetection();
	HRESULT Render_Fog(_wstring& strFinalTag);
	HRESULT Render_PostProcess_Distortion(_wstring& strFinalTag);
	HRESULT Render_PostProcess_Bloom(_wstring& strFinalTag);
	HRESULT Render_PostProcess_ColorCorrection(_wstring& strFinalTag);
	HRESULT Render_PostProcess_RadialBlur();
	HRESULT Render_PostProcess_Final(); // Render to BackBuffer
	HRESULT Render_Blur(); 
	HRESULT Render_UI_Anim();
	HRESULT Render_UI();

private:
	HRESULT Ready_DepthStencilBuffer(_uint iWidth, _uint iHeight, ID3D11DepthStencilView** ppOut, _uint iArraySize = 1);
	HRESULT SetUp_Viewport(_uint iWidth, _uint iHeight);

	HRESULT Ready_Components();
	HRESULT Ready_RenderTargets();
	HRESULT Ready_MRTs();
	HRESULT Ready_RenderPasses();

	HRESULT Bind_Matrices(CShader* pShaderCom) const;

#ifdef _DEBUG
private:
	HRESULT Ready_DebugRenders();
	HRESULT Ready_DebugDepthStencilState();
	HRESULT Render_Debug();
#endif // _DEBUG


	void Clear();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
