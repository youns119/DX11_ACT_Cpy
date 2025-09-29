#include "pch.h"
#include "Quest_Manager.h"
#include "ShellHouse.h"
#include "Player.h"

#include "GameInstance.h"

CQuest_Manager::CQuest_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CQuest_Manager::CQuest_Manager(const CQuest_Manager& Prototype)
	: super(Prototype)
{
}

HRESULT CQuest_Manager::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuest_Manager::Initialize(void* pArg)
{
	CQuest_Manager::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}	

void CQuest_Manager::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CQuest_Manager::Update(_float fTimeDelta)
{		
	super::Update(fTimeDelta);
}

void CQuest_Manager::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

CQuest_Manager* CQuest_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Manager* pInstance = new CQuest_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CQuest_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CQuest_Manager::Clone(void* pArg)
{
	CQuest_Manager* pInstance = new CQuest_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CQuest_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest_Manager::Free()
{
	super::Free();
}

