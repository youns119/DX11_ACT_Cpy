#include "pch.h"
#include "UI_Debuff.h"

#include "GameInstance.h"
#include "FPlayerStat.h"

#include "UI_Texture.h"
#include "UI_FixStretch.h"
#include "UI_FixStretchBar.h"

CUI_Debuff::CUI_Debuff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Debuff::CUI_Debuff(const CUI_Debuff& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Debuff::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Debuff::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	m_pPlayerStat = pDesc->pPlayerStat;

	m_fMaxFearGauge = m_pPlayerStat->fFearMaxGauge;

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CUI_Debuff::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Debuff::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CUI_Debuff::Late_Update(_float fTimeDelta)
{
	if (m_pPlayerStat->fFearGauge > 0.f)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

		super::Late_Update(fTimeDelta);
	}	
}

HRESULT CUI_Debuff::Render()
{    
	return S_OK;
}

HRESULT CUI_Debuff::Ready_PartObject()
{
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Debuff/UI_Icon.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Icon", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	/* Border */
	CUI_FixStretch::DESC FixStretchDesc{};
	{
		FixStretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Debuff/UI_Border.dat";

		FixStretchDesc.fChangeRatio = 1.f;
		FixStretchDesc.bMiddle = false;
		
		FixStretchDesc.fStartFactor = 150.f;
		FixStretchDesc.fEndFactor = 150.f;
		FixStretchDesc.fStretchSpeed = 10.f;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretch", &FixStretchDesc)))
		return E_FAIL;

	/* Bar */
	CUI_FixStretchBar::DESC FixStretchBarDesc{};
	{
		FixStretchBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Debuff/UI_Bar.dat";

		FixStretchBarDesc.fChangeRatio = 1.f;
		FixStretchBarDesc.bMiddle = false;

		FixStretchBarDesc.fStartFactor = 300.f;
		FixStretchBarDesc.fEndFactor = 300.f;
		FixStretchBarDesc.fStretchSpeed = 10.f;

		FixStretchBarDesc.pCur = &m_pPlayerStat->fFearGauge;
		FixStretchBarDesc.pMax = &m_fMaxFearGauge;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Bar", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretchBar", &FixStretchBarDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Debuff* CUI_Debuff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Debuff* pInstance = new CUI_Debuff(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Debuff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Debuff::Clone(void* pArg)
{
	CUI_Debuff* pInstance = new CUI_Debuff(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Debuff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Debuff::Free()
{
    CUIObject::Free();
}

