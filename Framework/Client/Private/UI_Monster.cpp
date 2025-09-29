#include "pch.h"
#include "UI_Monster.h"

#include "GameInstance.h"
#include "UI_FixStretchBar.h"
#include "UI_BallanceBorder.h"
#include "UI_BallanceBar.h"
#include "UI_Locked.h"
#include "UI_Peril.h"

CUI_Monster::CUI_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Monster::CUI_Monster(const CUI_Monster& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Monster::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Monster::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	m_pEnemyStats = pDesc->pEnemyStats;
	m_pMonsterWorld = pDesc->pMonsterWorld;

	m_pIsAggroed = pDesc->pIsAggroed;
	m_pIsDead = pDesc->pIsDead;

	m_pIsGrab = pDesc->pIsGrab;
	m_pIsUnblockable = pDesc->pIsUnblockable;

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CUI_Monster::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Monster::Update(_float fTimeDelta)
{
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

	super::Update(fTimeDelta);
}

void CUI_Monster::Late_Update(_float fTimeDelta)
{
	if (*m_pIsDead)
		return;
	
	if (!*m_pIsAggroed)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	
	// 풀피가 아님 -> 플레이어가 때렸다
	if(m_pEnemyStats->fCurHP != m_pEnemyStats->fMaxHP)
		super::Late_Update(fTimeDelta);
}

HRESULT CUI_Monster::Render()
{
    return S_OK;
}

HRESULT CUI_Monster::Ready_PartObject()
{
	/* MonsterHpBar */
	CUI_FixStretchBar::DESC FixStretchBarDesc{};
	{
		FixStretchBarDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = m_pMonsterWorld;
		FixStretchBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Monster/UI_MonsterHpBar.dat";

		FixStretchBarDesc.fChangeRatio = 1.f;
		FixStretchBarDesc.bMiddle = true;
		FixStretchBarDesc.fStartFactor = 100.f;
		FixStretchBarDesc.fEndFactor = 100.f;

		FixStretchBarDesc.pCur = &m_pEnemyStats->fCurHP;
		FixStretchBarDesc.pMax = &m_pEnemyStats->fMaxHP;

		FixStretchBarDesc.UIDesc.bInWorld = true;
		FixStretchBarDesc.UIDesc.fWorldOffsetY = 2.5f;
	}
	if (FAILED(Add_PartObject(L"Part_UI_MonsterHpBar", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretchBar", &FixStretchBarDesc)))
		return E_FAIL;

	/* MonsterBallanceBar */
	CUI_BallanceBar::DESC BallanceBarDesc{};
	{
		BallanceBarDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = m_pMonsterWorld;
		BallanceBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Monster/UI_MonsterBallanceBar.dat";

		BallanceBarDesc.fChangeRatio = 1.f;
		BallanceBarDesc.fEndFactor = 100.f;

		BallanceBarDesc.pCur = &m_pEnemyStats->fCurBP;
		BallanceBarDesc.pMax = &m_pEnemyStats->fMaxBP;

		BallanceBarDesc.UIDesc.bInWorld = true;
		BallanceBarDesc.UIDesc.fWorldOffsetY = 2.4f;
		BallanceBarDesc.UIDesc.isActive = true;
	}
	if (FAILED(Add_PartObject(L"Part_UI_MonsterBallanceBar", LEVEL_STATIC, L"Prototype_GameObject_UI_BallanceBar", &BallanceBarDesc)))
		return E_FAIL;

	m_pBallanceBar = static_cast<CUI_BallanceBar*>(Find_PartObject({ L"Part_UI_MonsterBallanceBar" }));

	/* MonsterBallanceBorder */
	CUI_BallanceBorder::DESC BallanceBorderDesc{};
	{
		BallanceBorderDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = m_pMonsterWorld;
		BallanceBorderDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Monster/UI_MonsterBallanceBorder.dat";

		BallanceBorderDesc.bMiddle = true;
		BallanceBorderDesc.fStretchFactor = 50.f;

		BallanceBorderDesc.UIDesc.bInWorld = true;
		BallanceBorderDesc.UIDesc.fWorldOffsetY = 2.5f;

		BallanceBorderDesc.UIDesc.isActive = false;
	}
	if (FAILED(Add_PartObject(L"Part_UI_MonsterBallanceBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_BallanceBorder", &BallanceBorderDesc)))
		return E_FAIL;

	m_pBallanceBorder = static_cast<CUI_BallanceBorder*>(Find_PartObject({ L"Part_UI_MonsterBallanceBorder" }));

	/* Locked */
	CUI_Locked::DESC LockedDesc{};
	{
		LockedDesc.UIDesc.UIObjectDesc.GameObjectDesc.pParentWorldMatrix = m_pMonsterWorld;
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
		PerilDesc.pIsUnblockable = m_pIsUnblockable;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Peril", LEVEL_STATIC, L"Prototype_GameObject_UI_Peril", &PerilDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Monster* CUI_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Monster* pInstance = new CUI_Monster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Monster::Clone(void* pArg)
{
	CUI_Monster* pInstance = new CUI_Monster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Monster::Free()
{
    CUIObject::Free();
}

