#include "RenderTarget_Manager.h"

#include "RenderTarget.h"

CRenderTarget_Manager::CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget_Manager::Initialize()
{
	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize, _uint iMipLevel)
{
	if (nullptr != Find_RenderTarget(strRenderTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor, iArraySize, iMipLevel);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_RT_To_Shader(const _wstring& strRenderTargetTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_RT_To_Shader(pShader, pConstantName);
}

HRESULT CRenderTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;


	if (m_MRTs.find(strMRTTag) == m_MRTs.end())
	{
		vector<CRenderTarget*> MRT;
		MRT.reserve(8);

		m_MRTs.emplace(strMRTTag, MRT);
		m_MRTResolutions.emplace(strMRTTag, pRenderTarget->Get_Resolution());
	}

	_uint2 vMRTResolution = m_MRTResolutions.at(strMRTTag);
	_uint2 vRenderTargetResolution = pRenderTarget->Get_Resolution();
	if (vMRTResolution.x != vRenderTargetResolution.x || vMRTResolution.y != vRenderTargetResolution.y)
		return E_FAIL;

	m_MRTs.at(strMRTTag).push_back(pRenderTarget);
	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(const _wstring& strMRTTag, _bool isClear, ID3D11DepthStencilView* pDSV)
{
	m_pContext->OMGetRenderTargets(1, &m_pBackBuffer, &m_pOriginDSV); // 이 함수 호출은 initialize 에서 해도 되지 않냐?

	//ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
	//nullptr
	//};
	array<ID3D11ShaderResourceView*, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT> SRVs {};
	SRVs.fill(nullptr);

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, SRVs.data());

	vector<CRenderTarget*>& MRTs = Find_MRT(strMRTTag);

	array<ID3D11RenderTargetView*, 8> RTVs {};
	RTVs.fill(nullptr);

	_uint iNumRenderTargets { 0 };
	for (auto& pRenderTarget : MRTs)
	{
		if (isClear)
			pRenderTarget->Clear();

		RTVs[iNumRenderTargets] = pRenderTarget->Get_RTV();
		++iNumRenderTargets;
	}

	if (nullptr != pDSV)
	{
		m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		m_pContext->OMSetRenderTargets(static_cast<_uint>(RTVs.size()), RTVs.data(), pDSV);
	}
	else
		m_pContext->OMSetRenderTargets(static_cast<_uint>(RTVs.size()), RTVs.data(), m_pOriginDSV);


	const _uint2 MRTResolution = m_MRTResolutions.find(strMRTTag)->second;
	if (MRTResolution.x != m_iViewportWidth || MRTResolution.y != m_iViewportHeight)
	{
		m_iViewportWidth = MRTResolution.x;
		m_iViewportHeight = MRTResolution.y;

		SetUp_Viewport(m_iViewportWidth, m_iViewportHeight);
	}

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBuffer, m_pOriginDSV);

	Safe_Release(m_pOriginDSV);
	Safe_Release(m_pBackBuffer);

	return S_OK;
}

void CRenderTarget_Manager::Copy_RT_Resource(const _wstring& strRenderTargetTag, ID3D11Texture2D* pTexture2D)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);

	pRenderTarget->Copy_Resource(pTexture2D);
}

#ifdef _DEBUG
HRESULT CRenderTarget_Manager::Ready_RT_Debug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CRenderTarget_Manager::Render_MRT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	vector<CRenderTarget*>& MRT = Find_MRT(strMRTTag);

	for (auto& pRenderTarget : MRT)
	{
		if (nullptr == pRenderTarget)
			return E_FAIL;

		pRenderTarget->Render_Debug(pShader, pVIBuffer);
	}

	return S_OK;
}
#endif

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const _wstring& strRenderTagetTag)
{
	auto iter = m_RenderTargets.find(strRenderTagetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

vector<CRenderTarget*>& CRenderTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
	auto iter = m_MRTs.find(strMRTTag);

	// 여기서 처리를 어떻게 해줘야해?

	return iter->second;
}

void CRenderTarget_Manager::SetUp_Viewport(_uint iWidth, _uint iHeight)
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iWidth;
	ViewPortDesc.Height = (_float)iHeight;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
}

CRenderTarget_Manager* CRenderTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderTarget_Manager* pInstance = new CRenderTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CRenderTarget_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget_Manager::Free()
{
	super::Free();

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
