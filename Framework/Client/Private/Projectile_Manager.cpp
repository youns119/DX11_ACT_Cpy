#include "pch.h"
#include "Projectile_Manager.h"

#include "GameInstance.h"

#include "Projectile_Fizzle.h"
#include "Projectile_Grapple.h"
#include "Projectile_Urchin.h"

CProjectile_Manager::CProjectile_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CProjectile_Manager::CProjectile_Manager(const CProjectile_Manager& Prototype)
	: super(Prototype)
{
}

HRESULT CProjectile_Manager::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_Manager::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)) ||
		FAILED(Ready_Projectiles()))
		return E_FAIL;

	return S_OK;
}

void CProjectile_Manager::Priority_Update(_float fTimeDelta)
{
	for(_uint i = 0; i < static_cast<_uint>(PROJECTILE::PROJECTILE_END); i++)
		for (auto& Projectile : m_vecProjectile[i])
			if (Projectile->Get_Active())
				Projectile->Priority_Update(fTimeDelta);
}

void CProjectile_Manager::Update(_float fTimeDelta)
{
	for (_uint i = 0; i < static_cast<_uint>(PROJECTILE::PROJECTILE_END); i++)
		for (auto& Projectile : m_vecProjectile[i])
			if (Projectile->Get_Active())
				Projectile->Update(fTimeDelta);
}

void CProjectile_Manager::Late_Update(_float fTimeDelta)
{
	for (_uint i = 0; i < static_cast<_uint>(PROJECTILE::PROJECTILE_END); i++)
		for (auto& Projectile : m_vecProjectile[i])
			if (Projectile->Get_Active())
				Projectile->Late_Update(fTimeDelta);
}

void CProjectile_Manager::Final_Update(_float fTimeDelta)
{
	for (_uint i = 0; i < static_cast<_uint>(PROJECTILE::PROJECTILE_END); i++)
		for (auto& Projectile : m_vecProjectile[i])
			if (Projectile->Get_Active())
				Projectile->Final_Update(fTimeDelta);
}

HRESULT CProjectile_Manager::Render()
{
	for (_uint i = 0; i < static_cast<_uint>(PROJECTILE::PROJECTILE_END); i++)
		for (auto& Projectile : m_vecProjectile[i])
			if (Projectile->Get_Active())
				if (FAILED(Projectile->Render()))
					return E_FAIL;

	return S_OK;
}

CProjectile* CProjectile_Manager::Get_Projectile(PROJECTILE eProjectile)
{
	for (auto& Projectile : m_vecProjectile[static_cast<_uint>(eProjectile)])
		if (!Projectile->Get_Active())
			return Projectile;

	return nullptr;
}

void CProjectile_Manager::Spawn(PROJECTILE eProjectile, _uint iNum, const FProjectileStat& tProjectileStat)
{
	_uint iCount{};

	for (auto& Projectile : m_vecProjectile[static_cast<_uint>(eProjectile)])
	{
		if (!Projectile->Get_Active())
		{
			Projectile->Activate(tProjectileStat);
			iCount++;
		}

		if (iCount >= iNum) break;
	}
}

void CProjectile_Manager::Deactivate_All()
{
	for (_uint i = 0; i < static_cast<_uint>(PROJECTILE::PROJECTILE_END); i++)
		for (auto& Projectile : m_vecProjectile[i])
			Projectile->Deactivate();
}

HRESULT CProjectile_Manager::Ready_Projectiles()
{
	// Fizzle
	{
		for (_uint i = 0; i < 100; i++)
		{
			CProjectile_Fizzle* pInstance = static_cast<CProjectile_Fizzle*>(m_pGameInstance->Clone_Prototype
			(
				LEVEL_STATIC,
				L"Prototype_GameObject_Projectile_Fizzle"
			));
			if (pInstance == nullptr)
				return E_FAIL;

			m_vecProjectile[static_cast<_uint>(PROJECTILE::PROJECTILE_FIZZLE)].push_back(pInstance);
		}
	}

	// Grapple
	{
		for (_uint i = 0; i < 10; i++)
		{
			CProjectile_Grapple* pInstance = static_cast<CProjectile_Grapple*>(m_pGameInstance->Clone_Prototype
			(
				LEVEL_STATIC,
				L"Prototype_GameObject_Projectile_Grapple"
			));
			if (pInstance == nullptr)
				return E_FAIL;

			m_vecProjectile[static_cast<_uint>(PROJECTILE::PROJECTILE_GRAPPLE)].push_back(pInstance);
		}
	}

	// Urchin
	{
		for (_uint i = 0; i < 20; i++)
		{
			CProjectile_Urchin* pInstance = static_cast<CProjectile_Urchin*>(m_pGameInstance->Clone_Prototype
			(
				LEVEL_STATIC,
				L"Prototype_GameObject_Projectile_Urchin"
			));
			if (pInstance == nullptr)
				return E_FAIL;

			m_vecProjectile[static_cast<_uint>(PROJECTILE::PROJECTILE_URCHIN)].push_back(pInstance);
		}
	}

	return S_OK;
}

CProjectile_Manager* CProjectile_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Manager* pInstance = new CProjectile_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CProjectile_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CProjectile_Manager::Clone(void* pArg)
{
	CProjectile_Manager* pInstance = new CProjectile_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CProjectile_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Manager::Free()
{
	super::Free();

	for (_uint i = 0; i < static_cast<_uint>(PROJECTILE::PROJECTILE_END); i++)
		for (auto& Projectile : m_vecProjectile[i])
			Safe_Release(Projectile);
}