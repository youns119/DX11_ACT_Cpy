#include "pch.h"
#include "Projectile.h"

#include "GameInstance.h"
#include "Collider.h"

CProjectile::CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CProjectile::CProjectile(const CProjectile& Prototype)
	: super(Prototype)
{
}

HRESULT CProjectile::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CProjectile::Priority_Update(_float fTimeDelta)
{
	m_tProjectileStat.fLifeTime -= fTimeDelta;
	if (m_tProjectileStat.fLifeTime <= 0.f)
	{
		Deactivate();
		return;
	}

	super::Priority_Update(fTimeDelta);
}

void CProjectile::Update(_float fTimeDelta)
{
	m_pMovementCom->Update(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	super::Update(fTimeDelta);
}

void CProjectile::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	// DebugComponent µî·Ï
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif

	m_pMovementCom->Late_Update(fTimeDelta);

	super::Late_Update(fTimeDelta);
}

void CProjectile::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CProjectile::Render()
{
	return S_OK;
}

void CProjectile::Activate(const FProjectileStat& tProjectileStat)
{
	m_bActive = true;
	m_pColliderCom->Set_Active(true);

	m_tProjectileStat = tProjectileStat;
}

void CProjectile::Deactivate()
{
	m_bActive = false;
	m_pColliderCom->Set_Active(false);
}

void CProjectile::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
}

void CProjectile::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CProjectile::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CProjectile::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMovementCom);
}