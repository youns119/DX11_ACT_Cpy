#include "pch.h"
#include "ShellSkill_Fizzle.h"

#include "GameInstance.h"

#include "Player.h"
#include "Shell_Part.h"
#include "Projectile_Manager.h"
#include "Projectile_Fizzle.h"

CShellSkill_Fizzle::CShellSkill_Fizzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

CShellSkill_Fizzle::CShellSkill_Fizzle(const CShellSkill_Fizzle& Prototype)
    : super(Prototype)
{
}

HRESULT CShellSkill_Fizzle::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	m_fSkillDamage = 5.f;

    return S_OK;
}

HRESULT CShellSkill_Fizzle::Initialize(void* pArg)
{
	CShellSkill::DESC* pDesc = static_cast<CShellSkill::DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CShellSkill_Fizzle::Priority_Update(_float fTimeDelta)
{
    super::Priority_Update(fTimeDelta);
}

void CShellSkill_Fizzle::Update(_float fTimeDelta)
{
    super::Update(fTimeDelta);
}

void CShellSkill_Fizzle::Late_Update(_float fTimeDelta)
{
    super::Late_Update(fTimeDelta);
}

void CShellSkill_Fizzle::Final_Update(_float fTimeDelta)
{
    super::Final_Update(fTimeDelta);
}

HRESULT CShellSkill_Fizzle::Render()
{
	return S_OK;
}

void CShellSkill_Fizzle::Use_Skill()
{
	super::Use_Skill();

	CProjectile_Manager* pManager = static_cast<CProjectile_Manager*>(m_pGameInstance->Find_GameObject
	(
		m_pGameInstance->Get_CurrentLevelIndex(),
		L"Layer_Projectile"
	));

	FProjectileStat tProjectileStat{};

	_vec3 vPos = static_cast<CTransform*>(Get_ShellOwner()->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
	tProjectileStat.vSrc = vPos._float3();
	tProjectileStat.fAccel = 100.f;
	tProjectileStat.fDecel = 35.f;
	tProjectileStat.fForce = 10.f;
	tProjectileStat.fForceFriction = 20.f;
	tProjectileStat.fMinSpeed = 0.5f;
	tProjectileStat.fMaxSpeed = 50.f;
	tProjectileStat.fLifeTime = 7.f;
	tProjectileStat.fDamage = Calculate_Damage();

	pManager->Spawn(PROJECTILE::PROJECTILE_FIZZLE, 15, tProjectileStat);
}

CShellSkill_Fizzle* CShellSkill_Fizzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShellSkill_Fizzle* pInstance = new CShellSkill_Fizzle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CShellSkill_Fizzle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CShellSkill_Fizzle::Clone(void* pArg)
{
	CShellSkill_Fizzle* pInstance = new CShellSkill_Fizzle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CShellSkill_Fizzle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShellSkill_Fizzle::Free()
{
	super::Free();
}