#include "pch.h"
#include "ActiveSkill_Urchin.h"

#include "GameInstance.h"

#include "Projectile_Manager.h"
#include "Player.h"

CActiveSkill_Urchin::CActiveSkill_Urchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CActiveSkill_Urchin::CActiveSkill_Urchin(const CActiveSkill_Urchin& Prototype)
	: super(Prototype)
{
}

HRESULT CActiveSkill_Urchin::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActiveSkill_Urchin::Initialize(void* pArg)
{
	CActiveSkill::DESC* pDesc = static_cast<CActiveSkill::DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CActiveSkill_Urchin::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CActiveSkill_Urchin::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CActiveSkill_Urchin::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CActiveSkill_Urchin::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CActiveSkill_Urchin::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CActiveSkill_Urchin::Use_Skill()
{
	super::Use_Skill();

    CProjectile_Manager* pManager = static_cast<CProjectile_Manager*>(m_pGameInstance->Find_GameObject
    (
        m_pGameInstance->Get_CurrentLevelIndex(),
        L"Layer_Projectile"
    ));

    FProjectileStat tProjectileStat{};

    CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Find_Component(g_strTransformTag));
    _vec3 vOriginPos = pTransform->Get_State(CTransform::STATE::POSITION);

    tProjectileStat.vSrc = vOriginPos._float3();
    tProjectileStat.fForce = 40.f;
	tProjectileStat.fLifeTime = 100.f;
	tProjectileStat.fDamage = 70.f;

    pManager->Spawn(PROJECTILE::PROJECTILE_URCHIN, 1, tProjectileStat);
}

CActiveSkill_Urchin* CActiveSkill_Urchin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActiveSkill_Urchin* pInstance = new CActiveSkill_Urchin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CActiveSkill_Urchin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CActiveSkill_Urchin::Clone(void* pArg)
{
	CActiveSkill_Urchin* pInstance = new CActiveSkill_Urchin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CActiveSkill_Urchin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActiveSkill_Urchin::Free()
{
	super::Free();
}