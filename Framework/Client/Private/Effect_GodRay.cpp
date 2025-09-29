#include "pch.h"
#include "Effect_GodRay.h"

#include "GameInstance.h"

CEffect_GodRay::CEffect_GodRay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEffect_GodRay::CEffect_GodRay(const CEffect_GodRay& Prototype)
	: super(Prototype)
{
}

HRESULT CEffect_GodRay::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_GodRay::Initialize(void* pArg)
{
	super::DESC* pDesc = static_cast<super::DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components((LEVELID)pDesc->eLevelID, pDesc->BufferDesc)))
		return E_FAIL;

	//m_pVIBufferCom->Progress(0.f);

	return S_OK;
}

void CEffect_GodRay::Priority_Update(_float fTimeDelta)
{
}

void CEffect_GodRay::Update(_float fTimeDelta)
{
	if (false == m_isActive)
		return;

	m_fActiveDelayAcc -= fTimeDelta;
	if (m_fActiveDelayAcc > 0.f)
		return;

	Compute_CamDistance();
	m_pVIBufferCom->Progress(fTimeDelta);
}

void CEffect_GodRay::Late_Update(_float fTimeDelta)
{
	if (false == m_isActive)
		return;

	if (m_fActiveDelayAcc > 0.f)
		return;

	super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::WEIGHTEDBLEND, this);
}

HRESULT CEffect_GodRay::Render()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	m_vCamPosition = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMulColor", &m_vMulColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vAddColor", &m_vAddColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsLoop", &m_bIsLoop, sizeof(_float))))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_InputAssembler();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_GodRay::Render_Revealage()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	m_vCamPosition = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMulColor", &m_vMulColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vAddColor", &m_vAddColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsLoop", &m_bIsLoop, sizeof(_float))))
		return E_FAIL;


	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_InputAssembler();

	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CEffect_GodRay::Ready_Components(LEVELID eLevelID, CVIBuffer_Particle_Point::DESC& Desc)
{
	/* Com_Shader */
	if (FAILED(Add_Component(eLevelID, L"Prototype_Component_Shader_VtxPoint_GodRay",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(eLevelID, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

CEffect_GodRay* CEffect_GodRay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_GodRay* pInstance = new CEffect_GodRay(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CEffect_GodRay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CEffect_GodRay::Clone(void* pArg)
{
	CEffect_GodRay* pInstance = new CEffect_GodRay(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEffect_GodRay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_GodRay::Free()
{
	super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
