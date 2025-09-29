#include "pch.h"
#include "ActiveSkill.h"

#include "Player.h"
#include "Skill_DataBase.h"

CActiveSkill::CActiveSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CActiveSkill::CActiveSkill(const CActiveSkill& Prototype)
	: super(Prototype)
{
}

HRESULT CActiveSkill::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActiveSkill::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	m_pOwner = pDesc->pOwner;
	m_tActiveSkill = CSkill_DataBase::Get().Get_ActiveSkill(pDesc->eActiveSkill);

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CActiveSkill::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CActiveSkill::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CActiveSkill::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CActiveSkill::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CActiveSkill::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CActiveSkill::Use_Skill()
{
	m_pOwner->Get_PlayerStat()->fCurrSkillPoint -= m_tActiveSkill.fSkillPoint;
}

void CActiveSkill::Free()
{
	super::Free();
}