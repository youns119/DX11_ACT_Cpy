#include "RenderPass_RadialBlur.h"

#include "GameInstance.h"

CRenderPass_RadialBlur::CRenderPass_RadialBlur()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderPass_RadialBlur::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize)
{
	_uint iViewportWidth = vViewportSize.x;
	_uint iViewportHeight = vViewportSize.y;

	/* Ready_Components */
	m_pShaderCom_Blur = CShader::Create(pDevice, pContext, L"../../EngineSDK/Hlsl/Shader_Blur.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShaderCom_Blur)
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderPass_RadialBlur::Render(const RENDERPASS_PARAMS& Params)
{
	if (FAILED(m_pGameInstance->Begin_MRT(Params.strFinalMRTTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_To_Shader(Params.strFinalSRVTag, m_pShaderCom_Blur, "g_Texture")))
		return E_FAIL;

	if (FAILED(Bind_Matrices(m_pShaderCom_Blur, Params.pWorldMatrix, Params.pViewMatrix, Params.pProjMatrix)))
		return E_FAIL;

	if FAILED(m_pShaderCom_Blur->Bind_RawValue("g_RadialBlurParams", Params.pRadialBlurParams, sizeof(RADIALBLUR_PARAMS)))
		return E_FAIL;

	m_pShaderCom_Blur->Begin((_uint)SHADER_PASS_BLUR::RADIAL);

	Params.pVIBuffer->Bind_InputAssembler();
	Params.pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderPass_RadialBlur::Bind_Matrices(CShader* pShader, const _float4x4* pWorldMatrix, const _float4x4* pViewMatrix, const _float4x4* pProjMatrix)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", pWorldMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", pViewMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", pProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CRenderPass_RadialBlur* CRenderPass_RadialBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint2 vViewportSize)
{
	CRenderPass_RadialBlur* pInstance = new CRenderPass_RadialBlur();
	if (FAILED(pInstance->Initialize(pDevice, pContext, vViewportSize)))
	{
		MSG_BOX("Failed To Create : CRenderPass_RadialBlur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderPass_RadialBlur::Free()
{
	super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pShaderCom_Blur);
}
