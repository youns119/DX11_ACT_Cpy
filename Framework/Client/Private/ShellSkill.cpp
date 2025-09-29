#include "pch.h"
#include "ShellSkill.h"

#include "Player.h"
#include "Shell_Part.h"

CShellSkill::CShellSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CShellSkill::CShellSkill(const CShellSkill& Prototype)
	: super(Prototype)
	, m_fSkillDamage{ Prototype.m_fSkillDamage }
{
}

HRESULT CShellSkill::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShellSkill::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	m_pOwner = pDesc->pOwner;
	m_fSkillPoint = pDesc->fSkillPoint;
	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CShellSkill::Priority_Update(_float fTimeDelta)
{
}

void CShellSkill::Update(_float fTimeDelta)
{
}

void CShellSkill::Late_Update(_float fTimeDelta)
{
}

void CShellSkill::Final_Update(_float fTimeDelta)
{
}

HRESULT CShellSkill::Render()
{
	return S_OK;
}

CGameObject* CShellSkill::Get_ShellOwner()
{
	return m_pOwner->Get_Owner();
}

void CShellSkill::Use_Skill()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(Get_ShellOwner());
	if (pPlayer != nullptr)
		pPlayer->Get_PlayerStat()->fCurrSkillPoint -= m_fSkillPoint;
}

_float CShellSkill::Calculate_Damage()
{
	_float fDamage = m_fSkillDamage;
	CPlayer* pPlayer = static_cast<CPlayer*>(Get_ShellOwner());
	if (pPlayer != nullptr)
		fDamage = fDamage * ((5.f * (_float)pPlayer->Get_PlayerStat()->iMSG) / 100.f);

	return fDamage;
}

void CShellSkill::Free()
{
	super::Free();
}