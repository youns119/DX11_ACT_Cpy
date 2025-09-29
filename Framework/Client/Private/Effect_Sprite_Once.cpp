#include "pch.h"
#include "..\Public\Effect_Sprite_Once.h"

#include "GameInstance.h"

CEffect_Sprite_Once::CEffect_Sprite_Once(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteEffect{ pDevice, pContext }
{

}

CEffect_Sprite_Once::CEffect_Sprite_Once(const CEffect_Sprite_Once& Prototype)
	: CSpriteEffect{ Prototype }
{
}

HRESULT CEffect_Sprite_Once::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Sprite_Once::Initialize(void* pArg)
{
	super::DESC* pDesc = static_cast<super::DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->strTextureTag, (LEVELID)pDesc->eLevelID, pDesc->BufferDesc)))
		return E_FAIL;

	//m_pVIBufferCom->Progress(0.f);

	return S_OK;
}

void CEffect_Sprite_Once::Priority_Update(_float fTimeDelta)
{
	m_fActiveDelayAcc -= fTimeDelta;
	if (m_fActiveDelayAcc > 0.f)
		return;

}

void CEffect_Sprite_Once::Update(_float fTimeDelta)
{
	m_fActiveDelayAcc -= fTimeDelta;
	if (m_fActiveDelayAcc > 0.f)
		return;


	if (super::EFFECT_TYPE::SPRITE == m_iEffectType)
		m_vUVInterval = _float2(1.f / (_float)m_vNumAtlas.x, 1.f / (_float)m_vNumAtlas.y);

	m_pVIBufferCom->Progress(fTimeDelta);
	Clear_Info(fTimeDelta);
}

void CEffect_Sprite_Once::Late_Update(_float fTimeDelta)
{
	if (m_fActiveDelayAcc > 0.f)
		return;

	super::Late_Update(fTimeDelta);

	if (true == m_bBlendEnable)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::WEIGHTEDBLEND, this);
	else
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONLIGHT, this);
}

HRESULT CEffect_Sprite_Once::Render()
{
	if (m_pAdoptedMatrix == nullptr)
	{
		if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iShadingOption", &m_iShadingOption, sizeof(_uint))))
		return E_FAIL;

	if (m_iShadingOption & EFFECT_OPTION::MASK)
	{
		if (FAILED(m_pMaskTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_MaskTexture", m_iMaskNum)))
			return E_FAIL;
	}

	if (m_iShadingOption & EFFECT_OPTION::DISSOLVE)
	{
		if (FAILED(m_pNoiseTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_DissolveTexture", m_iDissolveNum)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveStart", &m_vDissolveStart, sizeof(_float2))))
			return E_FAIL;
	}

	if (m_iShadingOption & EFFECT_OPTION::EMISSIVE)
	{
		if (FAILED(m_pEmissiveTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_EmissiveTexture", m_iEmissiveNum)))
			return E_FAIL;
	}

	if (m_iShadingOption & EFFECT_OPTION::DISTORTION)
	{
		if (FAILED(m_pNoiseTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_DistortionTexture", m_iDistortionNum)))
			return E_FAIL;
	}

	m_vCamPosition = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMulColor", &m_vMulColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vAddColor", &m_vAddColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFinalSize", &m_fFinalSize, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsLoop", &m_bIsLoop, sizeof(_bool))))
		return E_FAIL;

	if (super::EFFECT_TYPE::SPRITE == m_iEffectType)
	{
		if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumAtlas", &m_vNumAtlas, sizeof(_int3))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVInterval", &m_vUVInterval, sizeof(_float2))))
			return E_FAIL;

		if (true == m_bBlendEnable)
			m_pShaderCom->Begin((_uint)SHADER_PASS_PARTICLE::BLENDACC_UV);
		else
			m_pShaderCom->Begin((_uint)SHADER_PASS_PARTICLE::SPRITE_UV);
	}
	else
	{
		if (true == m_bBlendEnable)
			m_pShaderCom->Begin((_uint)SHADER_PASS_PARTICLE::BLENDACC);
		else
			m_pShaderCom->Begin((_uint)SHADER_PASS_PARTICLE::DEFAULT);
	}

	m_pVIBufferCom->Bind_InputAssembler();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_Sprite_Once::Render_Revealage()
{
	if (m_pAdoptedMatrix == nullptr)
	{
		if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iShadingOption", &m_iShadingOption, sizeof(_uint))))
		return E_FAIL;

	if (m_iShadingOption & EFFECT_OPTION::MASK)
	{
		if (FAILED(m_pMaskTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_MaskTexture", m_iMaskNum)))
			return E_FAIL;
	}

	if (m_iShadingOption & EFFECT_OPTION::DISSOLVE)
	{
		if (FAILED(m_pNoiseTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_DissolveTexture", m_iNoiseNum)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveStart", &m_vDissolveStart, sizeof(_float2))))
			return E_FAIL;
	}

	if (m_iShadingOption & EFFECT_OPTION::EMISSIVE)
	{
		if (FAILED(m_pEmissiveTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_EmissiveTexture", m_iEmissiveNum)))
			return E_FAIL;
	}

	if (m_iShadingOption & EFFECT_OPTION::DISTORTION)
	{
		if (FAILED(m_pNoiseTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_DistortionTexture", m_iDistortionNum)))
			return E_FAIL;
	}

	m_vCamPosition = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMulColor", &m_vMulColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vAddColor", &m_vAddColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFinalSize", &m_fFinalSize, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsLoop", &m_bIsLoop, sizeof(_bool))))
		return E_FAIL;

	if (super::EFFECT_TYPE::SPRITE == m_iEffectType)
	{
		if (FAILED(m_pGameInstance->Bind_RT_To_Shader(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumAtlas", &m_vNumAtlas, sizeof(_int3))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVInterval", &m_vUVInterval, sizeof(_float2))))
			return E_FAIL;
		m_pShaderCom->Begin((_uint)SHADER_PASS_PARTICLE::REVEAL_UV);
	}
	else
		m_pShaderCom->Begin((_uint)SHADER_PASS_PARTICLE::REVEAL);

	m_pVIBufferCom->Bind_InputAssembler();

	m_pVIBufferCom->Render();
	return S_OK;
}

void CEffect_Sprite_Once::Clear_Info(_float fTimeDelta)
{
	m_fLifeTimeAcc += fTimeDelta;
	if (m_fLifeTimeAcc >= m_fMaxLifeTime)
	{
		super::Clear_Info(fTimeDelta);
		m_pVIBufferCom->Reset_Life();
	}
}

HRESULT CEffect_Sprite_Once::Ready_Components(const _wstring& strTextureTag, LEVELID eLevelID, CVIBuffer_Particle_Point::DESC& Desc)
{
	/* Com_Shader */
	if (FAILED(Add_Component(eLevelID, L"Prototype_Component_Shader_VtxPoint_Particle",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(eLevelID, strTextureTag, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(Add_Component(eLevelID, TEXT("Prototype_Component_Texture_EffMask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	if (FAILED(Add_Component(eLevelID, TEXT("Prototype_Component_Texture_EffNoise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;

	if (FAILED(Add_Component(eLevelID, TEXT("Prototype_Component_Texture_EffEmissive"),
		TEXT("Com_EmissiveTexture"), reinterpret_cast<CComponent**>(&m_pEmissiveTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(eLevelID, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

CEffect_Sprite_Once* CEffect_Sprite_Once::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Sprite_Once* pInstance = new CEffect_Sprite_Once(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CEffect_Sprite_Once");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Sprite_Once::Clone(void* pArg)
{
	CEffect_Sprite_Once* pInstance = new CEffect_Sprite_Once(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CEffect_Sprite_Once");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Sprite_Once::Free()
{
	super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pEmissiveTextureCom);
}
