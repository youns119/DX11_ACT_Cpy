#include "MeshEffect.h"

CMeshEffect::CMeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CMeshEffect::CMeshEffect(const CMeshEffect& Prototype)
	: CEffectObject{ Prototype }
{
}

HRESULT CMeshEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeshEffect::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (FAILED(__super::Initialize(&pDesc->EffectDesc)))
		return E_FAIL;

	m_bIsLoopEnd = false;

	m_bIsLoop = pDesc->bIsLoop;
	m_vCenter = pDesc->vCenter;
	m_fLifeTime = pDesc->fLifeTime;
	m_iMeshAnimOption = pDesc->iMeshAnimOption;

	m_bUVAnim = pDesc->bUVAnim;
	m_vUVStart = pDesc->vUVStart;
	m_vUVProgress = pDesc->vUVStart;
	m_vUVSpeed = pDesc->vUVSpeed;
	m_vUVTileCount = pDesc->vUVTileCount;

	m_fMaxTrailLength = pDesc->fMaxTrailLength;

	return S_OK;
}

void CMeshEffect::Priority_Update(_float fTimeDelta)
{
}

void CMeshEffect::Update(_float fTimeDelta)
{
}

void CMeshEffect::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

HRESULT CMeshEffect::Render()
{
	return S_OK;
}

HRESULT CMeshEffect::Save_Effect(HANDLE hFile, _ulong& dwByte)
{
	WriteFile(hFile, &m_bIsLoop, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_iMeshAnimOption, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_fLifeTime, sizeof(_float), &dwByte, nullptr);
	WriteFile(hFile, &m_vCenter, sizeof(_float3), &dwByte, nullptr);
			  
	WriteFile(hFile, &m_bUVAnim, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_vUVStart, sizeof(_float2), &dwByte, nullptr);
	WriteFile(hFile, &m_vUVSpeed, sizeof(_float2), &dwByte, nullptr);
	WriteFile(hFile, &m_vUVTileCount, sizeof(_float2), &dwByte, nullptr);
	WriteFile(hFile, &m_fMaxTrailLength, sizeof(_float), &dwByte, nullptr);

	super::Save_Effect(hFile, dwByte);
	return S_OK;
}

void CMeshEffect::Clear_Info(_float fTimeDelta)
{
	super::Clear_Info(fTimeDelta);
	m_bIsLoopEnd = false;
}

void CMeshEffect::Free()
{
	__super::Free();
}
