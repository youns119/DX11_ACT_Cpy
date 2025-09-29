#include "pch.h"
#include "ActiveSkill_BobbitTrap.h"

#include "GameInstance.h"

CActiveSkill_BobbitTrap::CActiveSkill_BobbitTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CActiveSkill_BobbitTrap::CActiveSkill_BobbitTrap(const CActiveSkill_BobbitTrap& Prototype)
	: super(Prototype)
{
}

HRESULT CActiveSkill_BobbitTrap::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActiveSkill_BobbitTrap::Initialize(void* pArg)
{
	CActiveSkill::DESC* pDesc = static_cast<CActiveSkill::DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)) ||
		FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CActiveSkill_BobbitTrap::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CActiveSkill_BobbitTrap::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CActiveSkill_BobbitTrap::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CActiveSkill_BobbitTrap::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CActiveSkill_BobbitTrap::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CActiveSkill_BobbitTrap::Use_Skill()
{
	super::Use_Skill();
}

HRESULT CActiveSkill_BobbitTrap::Ready_PartObject()
{
	return S_OK;
}

CActiveSkill_BobbitTrap* CActiveSkill_BobbitTrap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActiveSkill_BobbitTrap* pInstance = new CActiveSkill_BobbitTrap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CActiveSkill_BobbitTrap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CActiveSkill_BobbitTrap::Clone(void* pArg)
{
	CActiveSkill_BobbitTrap* pInstance = new CActiveSkill_BobbitTrap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CActiveSkill_BobbitTrap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActiveSkill_BobbitTrap::Free()
{
	super::Free();
}