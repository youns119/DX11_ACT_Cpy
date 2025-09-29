#include "SpriteEffect.h"

CSpriteEffect::CSpriteEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CSpriteEffect::CSpriteEffect(const CSpriteEffect& Prototype)
	: CEffectObject{ Prototype }
{
}

HRESULT CSpriteEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpriteEffect::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (FAILED(__super::Initialize(&pDesc->EffectDesc)))
		return E_FAIL;


	m_fMaxLifeTime = pDesc->fMaxLifeTime;
	m_fLifeTimeAcc = 0.f;

	m_iPattern = pDesc->iPattern;
	m_vNumAtlas.x = pDesc->vNumAtlas.x;
	m_vNumAtlas.y = pDesc->vNumAtlas.y;
	m_vNumAtlas.z = pDesc->iNumAtlas;
	m_vUVInterval = pDesc->vUVInterval;

	return S_OK;
}

void CSpriteEffect::Priority_Update(_float fTimeDelta)
{
}

void CSpriteEffect::Update(_float fTimeDelta)
{
}

void CSpriteEffect::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

HRESULT CSpriteEffect::Render()
{
	return S_OK;
}

HRESULT CSpriteEffect::Save_Effect(HANDLE hFile, _ulong& dwByte)
{
	WriteFile(hFile, &m_iPattern, sizeof(_int), &dwByte, nullptr);
	WriteFile(hFile, &m_vNumAtlas, sizeof(_int3), &dwByte, nullptr);
	WriteFile(hFile, &m_vUVInterval, sizeof(_float2), &dwByte, nullptr);

	super::Save_Effect(hFile, dwByte);
	return S_OK;
}

void CSpriteEffect::Clear_Info(_float fTimeDelta)
{
	m_fLifeTimeAcc = 0.f;
	super::Clear_Info(fTimeDelta);
}

void CSpriteEffect::Free()
{
	super::Free();
}
