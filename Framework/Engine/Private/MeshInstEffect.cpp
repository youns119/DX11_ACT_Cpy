#include "MeshInstEffect.h"

CMeshInstEffect::CMeshInstEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CMeshInstEffect::CMeshInstEffect(const CMeshInstEffect& Prototype)
	: CEffectObject{ Prototype }
{
}

HRESULT CMeshInstEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeshInstEffect::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (FAILED(__super::Initialize(&pDesc->EffectDesc)))
		return E_FAIL;

	m_fLifeTime = pDesc->fLifeTime;
	m_fLifeTimeAcc = 0.f;

	m_bUVAnim = pDesc->bUVAnim;
	m_vUVSpeed = pDesc->vUVSpeed;
	m_vUVStart = pDesc->vUVStart;
	m_vUVProgress = pDesc->vUVStart;
	m_vUVTileCount = pDesc->vUVTileCount;

	return S_OK;
}

void CMeshInstEffect::Priority_Update(_float fTimeDelta)
{
}

void CMeshInstEffect::Update(_float fTimeDelta)
{
}

void CMeshInstEffect::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

HRESULT CMeshInstEffect::Render()
{
	return S_OK;
}

HRESULT CMeshInstEffect::Save_Effect(HANDLE hFile, _ulong& dwByte)
{
	WriteFile(hFile, &m_bIsLoop, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_fLifeTime, sizeof(_float), &dwByte, nullptr);

	WriteFile(hFile, &m_bUVAnim, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_vUVStart, sizeof(_float2), &dwByte, nullptr);
	WriteFile(hFile, &m_vUVSpeed, sizeof(_float2), &dwByte, nullptr);
	WriteFile(hFile, &m_vUVTileCount, sizeof(_float2), &dwByte, nullptr);

	super::Save_Effect(hFile, dwByte);
	return S_OK;
}

void CMeshInstEffect::Free()
{
	__super::Free();
}
