#include "EffectObject.h"
#include "GameInstance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffectObject::CEffectObject(const CEffectObject& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CEffectObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
	// Æ®·£½ºÆû
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	DESC* pDesc = static_cast<DESC*>(pArg);

	m_bIsLoop = pDesc->bIsLoop;

	m_bBlendEnable = pDesc->bBlendEnable;
	m_fActiveDelay = pDesc->fActiveDelay;
	m_fActiveDelayAcc = m_fActiveDelay;
	m_iEffectType = pDesc->iEffectType;
	m_fFinalSize = pDesc->fFinalSize;
	m_vMulColor = pDesc->vMulColor;
	m_vAddColor = pDesc->vAddColor;

	m_iShadingOption = pDesc->iShadingOption;
	m_iMaskNum = pDesc->iMaskImageNum;
	m_iNoiseNum = pDesc->iNoiseImageNum;
	m_iDissolveNum = pDesc->iDissolveImageNum;
	m_iEmissiveNum = pDesc->iEmissiveImageNum;
	m_iDistortionNum = pDesc->iDistortionImageNum;
	m_vDissolveStart = pDesc->vDissolveStart;
	m_fDistortionDissolveStart = pDesc->fDistortionDissolveStart;

	m_isActive = false;

	return S_OK;
}

void CEffectObject::Priority_Update(_float fTimeDelta)
{
}

void CEffectObject::Update(_float fTimeDelta)
{
}

void CEffectObject::Late_Update(_float fTimeDelta)
{
	if (nullptr != m_pAdoptedMatrix)
	{
		if(nullptr != m_pParentWorldMatrix)
			XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMMatrixTranslation(m_pAdoptedMatrix->_41, m_pAdoptedMatrix->_42, m_pAdoptedMatrix->_43) * XMLoadFloat4x4(m_pParentWorldMatrix));
		else
			XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMMatrixTranslation(m_pAdoptedMatrix->_41, m_pAdoptedMatrix->_42, m_pAdoptedMatrix->_43));
	}
}

HRESULT CEffectObject::Render()
{
	return S_OK;
}

HRESULT CEffectObject::Save_Effect(HANDLE hFile, _ulong& dwByte)
{
	WriteFile(hFile, &m_bBlendEnable, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_fActiveDelay, sizeof(_float), &dwByte, nullptr);
	WriteFile(hFile, &m_fFinalSize, sizeof(_float), &dwByte, nullptr);
	WriteFile(hFile, &m_vMulColor, sizeof(_float3), &dwByte, nullptr);
	WriteFile(hFile, &m_vAddColor, sizeof(_float3), &dwByte, nullptr);
	WriteFile(hFile, &m_iShadingOption, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iMaskNum, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNoiseNum, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iDissolveNum, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iEmissiveNum, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iDistortionNum, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_vDissolveStart, sizeof(_float2), &dwByte, nullptr);
	WriteFile(hFile, &m_fDistortionDissolveStart, sizeof(_float), &dwByte, nullptr);

	return S_OK;
}

void CEffectObject::Clear_Info(_float fTimeDelta)
{
	m_isActive = false;
	m_pAdoptedMatrix = nullptr;
	m_pParentWorldMatrix = nullptr;
	m_fActiveDelayAcc = m_fActiveDelay;
}

void CEffectObject::Free()
{
	__super::Free();
}
