#include "pch.h"
#include "UI_Boss.h"

#include "GameInstance.h"
#include "UI_FixStretch.h"
#include "UI_FixStretchBar.h"
#include "UI_FixStretchBarEffect.h"
#include "UI_BallanceBorder.h"
#include "UI_BallanceBar.h"
#include "UI_Locked.h"
#include "UI_Peril.h"

#include "Enemy.h"

CUI_Boss::CUI_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Boss::CUI_Boss(const CUI_Boss& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Boss::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Boss::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;
	pDesc->UIObjectDesc.fZ = 0.01f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	m_pEnemyStats = pDesc->pEnemyStats;
	m_pIsAggroed = pDesc->pIsAggroed;
	m_pIsDead = pDesc->pIsDead;

	m_pIsGrab = pDesc->pIsGrab;
	m_pIs = pDesc->pIsUnblockable;

	m_pCenterMatrixPtr = pDesc->pCenterMatrixPtr;

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	lstrcpy(m_szName, pDesc->szName);

	return S_OK;
}

void CUI_Boss::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Boss::Update(_float fTimeDelta)
{
	if (*m_pIsDead)
	{
		m_bIsAggroed = false;
		m_pHpBorder->Set_IsActive(false);
		m_pHpBar->Set_IsActive(false);
		m_pBallanceBar->Set_IsActive(false);
		m_pBallanceBorder->Set_IsActive(false);
		return;
	}

	if (*m_pIsAggroed)
	{
		if (!m_bIsAggroed)
		{
			m_bIsAggroed = true;
			m_pHpBorder->Reset();
			m_pHpBar->Reset();
		}
	}
	else
	{
		m_bIsAggroed = false;
		m_pHpBorder->Set_IsActive(false);
		m_pHpBar->Set_IsActive(false);
		m_pBallanceBar->Set_IsActive(false);
		m_pBallanceBorder->Set_IsActive(false);
	}


	if (m_pEnemyStats->fCurBP == m_pEnemyStats->fMaxBP)
	{
		m_pBallanceBar->Set_IsActive(false);
		m_pBallanceBorder->Blink();
	}

	if (m_pEnemyStats->bIsCapsized || m_pEnemyStats->fCurBP == 0.f)
	{
		m_pBallanceBar->Set_IsActive(true);
		m_pBallanceBorder->Set_IsActive(false);
	}

	_matrix CenterMatrix = XMLoadFloat4x4(m_pCenterMatrixPtr);
	for (size_t i = 0; i < 3; ++i)
		CenterMatrix.r[i] = XMVector3Normalize(CenterMatrix.r[i]);
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * CenterMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	super::Update(fTimeDelta);
}

void CUI_Boss::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	
	super::Late_Update(fTimeDelta);	
}

HRESULT CUI_Boss::Render()
{
	if(m_pHpBorder->End_Stretch() && m_bIsAggroed && !*m_pIsDead)
		m_pGameInstance->Render_Font(L"Font_Main_40", m_szName, {g_iWindowSizeX * 0.5f, 50.f}, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.5f, SORTING::MIDDLE);
    
	return S_OK;
}

HRESULT CUI_Boss::Ready_PartObject()
{
	/* BossHpBorder */
	CUI_FixStretch::DESC FixStretchDesc{};
	{
		FixStretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Boss/UI_BossHpBorder.dat";

		FixStretchDesc.fChangeRatio = 1.f;
		FixStretchDesc.bMiddle = true;
		
		FixStretchDesc.fStartFactor = 1.f;
		FixStretchDesc.fEndFactor = 500.f;
		FixStretchDesc.fStretchSpeed = 100.f;

	}
	if (FAILED(Add_PartObject(L"Part_UI_BossHpBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretch", &FixStretchDesc)))
		return E_FAIL;

	m_pHpBorder = static_cast<CUI_FixStretch*>(Find_PartObject({ L"Part_UI_BossHpBorder" }));

	/* BossHpBar */
	CUI_FixStretchBar::DESC FixStretchBarDesc{};
	{
		FixStretchBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Boss/UI_BossHpBar.dat";

		FixStretchBarDesc.fChangeRatio = 2.f;
		FixStretchBarDesc.bMiddle = true;

		FixStretchBarDesc.fStartFactor = 1.f;
		FixStretchBarDesc.fEndFactor = 2000.f;
		FixStretchBarDesc.fStretchSpeed = 200.f;

		FixStretchBarDesc.pCur = &m_pEnemyStats->fCurHP;
		FixStretchBarDesc.pMax = &m_pEnemyStats->fMaxHP;
	}
	if (FAILED(Add_PartObject(L"Part_UI_BossHpBar", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretchBar", &FixStretchBarDesc)))
		return E_FAIL;

	m_pHpBar = static_cast<CUI_FixStretchBar*>(Find_PartObject({ L"Part_UI_BossHpBar" }));

	/* BossHpBarEffect */
	CUI_FixStretchBarEffect::DESC FixStretchBarEffectDesc{};
	{
		FixStretchBarEffectDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Boss/UI_BossHpBarEffect.dat";

		FixStretchBarEffectDesc.fChangeRatio = 2.f;
		FixStretchBarEffectDesc.bMiddle = true;

		FixStretchBarEffectDesc.fStartFactor = 1.f;
		FixStretchBarEffectDesc.fEndFactor = 2000.f;
		FixStretchBarEffectDesc.fStretchSpeed = 200.f;

		FixStretchBarEffectDesc.pCur = &m_pEnemyStats->fCurHP;
		FixStretchBarEffectDesc.pMax = &m_pEnemyStats->fMaxHP;

	}
	if (FAILED(Add_PartObject(L"Part_UI_BossHpBarEffect", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretchBarEffect", &FixStretchBarEffectDesc)))
		return E_FAIL;

	/* BossBallanceBar */
	CUI_BallanceBar::DESC BallanceBarDesc{};
	{
		BallanceBarDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		BallanceBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Boss/UI_BossBallanceBar.dat";

		BallanceBarDesc.fChangeRatio = 2.f;
		BallanceBarDesc.fEndFactor = 2000.f;

		BallanceBarDesc.pCur = &m_pEnemyStats->fCurBP;
		BallanceBarDesc.pMax = &m_pEnemyStats->fMaxBP;

		BallanceBarDesc.UIDesc.isActive = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_BossBallanceBar", LEVEL_STATIC, L"Prototype_GameObject_UI_BallanceBar", &BallanceBarDesc)))
		return E_FAIL;

	m_pBallanceBar = static_cast<CUI_BallanceBar*>(Find_PartObject({ L"Part_UI_BossBallanceBar" }));

	/* BossBallanceBorder */
	CUI_BallanceBorder::DESC BallanceBorderDesc{};
	{
		BallanceBorderDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		BallanceBorderDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Boss/UI_BossBallanceBorder.dat";

		BallanceBorderDesc.bMiddle = true;
		BallanceBorderDesc.fStretchFactor = 1330.f;

		BallanceBorderDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_BossBallanceBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_BallanceBorder", &BallanceBorderDesc)))
		return E_FAIL;

	m_pBallanceBorder = static_cast<CUI_BallanceBorder*>(Find_PartObject({ L"Part_UI_BossBallanceBorder" }));

	/* Locked */
	CUI_Locked::DESC LockedDesc{};
	{
		LockedDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
		LockedDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Locked.dat";

		LockedDesc.UIDesc.bInWorld = true;
		LockedDesc.pLocked = &m_pEnemyStats->bIsLockedOn;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Locked", LEVEL_STATIC, L"Prototype_GameObject_UI_Locked", &LockedDesc)))
		return E_FAIL;

	CUI_Peril::DESC PerilDesc{};
	{
		PerilDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_Peril.dat";
		PerilDesc.pIsGrab = m_pIsGrab;
		PerilDesc.pIsUnblockable = m_pIs;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Peril", LEVEL_STATIC, L"Prototype_GameObject_UI_Peril", &PerilDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Boss* CUI_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Boss* pInstance = new CUI_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Boss::Clone(void* pArg)
{
	CUI_Boss* pInstance = new CUI_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Boss::Free()
{
    CUIObject::Free();
}

