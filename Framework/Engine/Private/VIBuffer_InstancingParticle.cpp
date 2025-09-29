#include "VIBuffer_InstancingParticle.h"
#include "GameInstance.h"
#include "Shader_Compute.h"

CVIBuffer_InstancingParticle::CVIBuffer_InstancingParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CVIBuffer_InstancingParticle::CVIBuffer_InstancingParticle(const CVIBuffer_InstancingParticle& Prototype)
	: super(Prototype)
	, m_InstanceBufferDesc { Prototype.m_InstanceBufferDesc }
	, m_InstanceInitialData { Prototype.m_InstanceInitialData }
	, m_iInstanceVertexStride { Prototype.m_iInstanceVertexStride }
	, m_iNumIndexPerInstance { Prototype.m_iNumIndexPerInstance }
	, m_iNumInstance { Prototype.m_iNumInstance }
	, m_pVBInstance { Prototype.m_pVBInstance }
	, m_Speeds { Prototype.m_Speeds }
	, m_isLoop { Prototype.m_isLoop }
	, m_vPivot { Prototype.m_vPivot }
	, m_ParticleVertices { Prototype.m_ParticleVertices } 

{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_InstancingParticle::Initialize_Prototype(const DESC& pDesc)
{
	return S_OK;
}

HRESULT CVIBuffer_InstancingParticle::Initialize(void* pArg)
{
	Create_CS(m_iPattern);
	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialData, &m_pVBInstance);
}

HRESULT CVIBuffer_InstancingParticle::Bind_InputAssembler()
{
	if (nullptr == m_pContext || false == m_bIsRenderable)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffer[] = 
	{
		m_pVB,
		m_pVBInstance,
	};

	_uint				iVertexStrides[] = 
	{
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint				iOffsets[] = 
	{
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_InstancingParticle::Render()
{
	if (nullptr == m_pContext || false == m_bIsRenderable)
		return E_FAIL;

	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

_bool CVIBuffer_InstancingParticle::Progress(_float fTimeDelta)
{
	CONST_BUFFER Desc{};
	Desc.iNumInstance = m_iNumInstance;
	Desc.fTimeDelta = fTimeDelta;
	Desc.bIsLoop = m_isLoop ? 1 : 0;
	Desc.bIsLoopEnd = m_isLoopEnd ? 1 : 0;
	Desc.vPivot = _float4(m_vPivot.x, m_vPivot.y, m_vPivot.z, 1.f);

	m_bIsRenderable = true;
	if(FAILED(m_pComputeShader->Compute(0, 1, &Desc, sizeof(CONST_BUFFER), m_pVBInstance)))
		m_bIsRenderable = false;

	//if (m_iPattern == FRACTION)
	//{
	//	D3D11_MAPPED_SUBRESOURCE SubResource{};
	//	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	//	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);
	// 	m_pContext->Unmap(m_pVBInstance, 0);
	//}
	return true;
}

void CVIBuffer_InstancingParticle::Change_Pattern(_uint iPattern)
{
	if (m_iPattern != iPattern)
	{
		m_iPattern = iPattern;
		switch (m_iPattern)
		{
		case EXPLOSION:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Explosion");
			break;
		case DROP:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Drop");
			break;
		case RISE:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Rise");
			break;
		case FRACTION:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Fraction");
			break;
		case HOLD:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Hold");
			break;
		case RISE_WATER:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_WaterRise");
			break;		
		case CHARGING:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Charging");
			break;
		case RISE_WATER_ENV:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_WaterRiseEnv");
			break;		
		case ASH_FLOW:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_AshFlow");
			break;
		case CROSS_SPREAD:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_CrossSpread");
			break;
		case GOD_RAY:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_GodRay");
			break;		
		case EXPL_SLOWDOWN:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_ExplSlowDown");
			break;
		case EXPL_SCALEGRADIENT:
			m_pComputeShader->Change_Function(L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_ExplScaleGradient");
			break;
		default:
			break;
		}
	}	
}

void CVIBuffer_InstancingParticle::Reset_Life()
{
	CONST_BUFFER Desc{};
	Desc.iNumInstance = m_iNumInstance;
	Desc.fTimeDelta = 0.f;
	Desc.bIsLoop = m_isLoop ? 1 : 0;
	Desc.bIsLoopEnd = m_isLoopEnd ? 1 : 0;
	Desc.vPivot = _float4(m_vPivot.x, m_vPivot.y, m_vPivot.z, 0.f);

	m_isLoopEnd = false;
	m_pComputeShader->Init_Data(0, 1, &Desc, sizeof(CONST_BUFFER), m_pVBInstance);
}

void CVIBuffer_InstancingParticle::Create_CS(_uint iPattern)
{	
	switch (iPattern)
	{
	case EXPLOSION:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Explosion", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case DROP:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Drop", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case RISE:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Rise", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case FRACTION:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Fraction", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case HOLD:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Hold", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case RISE_WATER:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_WaterRise", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case CHARGING:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_Charging", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case RISE_WATER_ENV:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_WaterRiseEnv", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case ASH_FLOW:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_AshFlow", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case CROSS_SPREAD:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_CrossSpread", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	case GOD_RAY:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_GodRay", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;	
	case EXPL_SLOWDOWN:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_ExplSlowDown", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;	
	case EXPL_SCALEGRADIENT:
		m_pComputeShader = CShader_Compute::Create(m_pDevice, m_pContext, L"../../EngineSDK/Hlsl/Shader_Compute_Particles.hlsl", "CS_ExplScaleGradient", "CS_Clear", sizeof(VTXPARTICLE), m_iNumInstance, sizeof(CONST_BUFFER), m_ParticleVertices.data());
		break;
	default:
		break;
	}
}

void CVIBuffer_InstancingParticle::Free()
{
	super::Free();

	Safe_Release(m_pVBInstance);
	Safe_Release(m_pComputeShader);
}
