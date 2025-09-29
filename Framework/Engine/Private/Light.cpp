#include "Light.h"

#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;
    return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer) const
{
	if (!m_isActive)
		return S_OK;

	_uint iPassIndex = 0;
	
	switch (m_LightDesc.eType)
	{
	case LIGHT_TYPE::DIRECTIONAL:
		iPassIndex = (_uint)SHADER_PASS_LIGHTING::DIRECTIONAL;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
		break;

	case LIGHT_TYPE::POINT:
		iPassIndex = (_uint)SHADER_PASS_LIGHTING::POINT;

		/* 임시용 프로스텀 라이트 컬링 */
		if (false == CGameInstance::GetInstance()->Is_InFrustum_WorldSpace(XMLoadFloat4(&m_LightDesc.vPosition), m_LightDesc.fRange))
			return S_OK;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vAttenuation", &m_LightDesc.vAttenuation, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		break;

	case LIGHT_TYPE::SPOT:
		iPassIndex = (_uint)SHADER_PASS_LIGHTING::SPOT;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vAttenuation", &m_LightDesc.vAttenuation, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vCosCutOff", &m_LightDesc.vCosCutOff, sizeof(_float2))))
			return E_FAIL;
		break;
	default:
		break;
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_InputAssembler();
	pVIBuffer->Render();

	return S_OK;
}

void CLight::Toggle()
{
	m_isActive = !m_isActive;
}

void CLight::Turn_On()
{
	m_isActive = true;
}

void CLight::Turn_Off()
{
	m_isActive = false;
}


CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Create : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
