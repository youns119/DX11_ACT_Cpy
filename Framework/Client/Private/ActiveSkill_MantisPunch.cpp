#include "pch.h"
#include "ActiveSkill_MantisPunch.h"

#include "GameInstance.h"

#include "Skill_Topoda.h"

CActiveSkill_MantisPunch::CActiveSkill_MantisPunch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CActiveSkill_MantisPunch::CActiveSkill_MantisPunch(const CActiveSkill_MantisPunch& Prototype)
	: super(Prototype)
{
}

HRESULT CActiveSkill_MantisPunch::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActiveSkill_MantisPunch::Initialize(void* pArg)
{
	CActiveSkill::DESC* pDesc = static_cast<CActiveSkill::DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)) ||
		FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CActiveSkill_MantisPunch::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CActiveSkill_MantisPunch::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CActiveSkill_MantisPunch::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CActiveSkill_MantisPunch::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CActiveSkill_MantisPunch::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CActiveSkill_MantisPunch::Use_Skill()
{
	super::Use_Skill();

	m_pTopoda->Spawn();
}

HRESULT CActiveSkill_MantisPunch::Ready_PartObject()
{
	CSkill_Topoda::DESC tSkill{};
	tSkill.tGameObjectDesc.pParentWorldMatrix = m_pParentWorldMatrix;
	tSkill.pOwner = m_pOwner;

	if (FAILED(Add_PartObject
	(
		L"Part_Skill",
		LEVEL_STATIC,
		L"Prototype_GameObject_Skill_Topoda",
		&tSkill
	)))
		return E_FAIL;

	m_pTopoda = static_cast<CSkill_Topoda*>(Find_PartObject({ L"Part_Skill" }));

	return S_OK;
}

CActiveSkill_MantisPunch* CActiveSkill_MantisPunch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActiveSkill_MantisPunch* pInstance = new CActiveSkill_MantisPunch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CActiveSkill_MantisPunch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CActiveSkill_MantisPunch::Clone(void* pArg)
{
	CActiveSkill_MantisPunch* pInstance = new CActiveSkill_MantisPunch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CActiveSkill_MantisPunch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActiveSkill_MantisPunch::Free()
{
	super::Free();
}