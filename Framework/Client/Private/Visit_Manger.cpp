#include "pch.h"
#include "Visit_Manger.h"

#include "GameInstance.h"

CVisit_Manger::CVisit_Manger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CVisit_Manger::CVisit_Manger(const CVisit_Manger& Prototype)
	: super(Prototype)
{
}

HRESULT CVisit_Manger::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVisit_Manger::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CVisit_Manger::Is_Visit_FromStatic(const _wstring& strTag)
{
	CVisit_Manger* pObject = static_cast<CVisit_Manger*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_VisitManager"));
	assert(pObject);

	return pObject->Is_Visit(strTag);
}

_bool CVisit_Manger::Is_Visit(const _wstring& strTag)
{
	auto iter = m_VisitedTags.find(strTag);

	if (m_VisitedTags.end() != iter)
		return true;

	m_VisitedTags.insert(strTag);
	return false;
}

CVisit_Manger* CVisit_Manger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVisit_Manger* pInstance = new CVisit_Manger(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CVisit_Manger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CVisit_Manger::Clone(void* pArg)
{
	CVisit_Manger* pInstance = new CVisit_Manger(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CVisit_Manger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVisit_Manger::Free()
{
	super::Free();

	m_VisitedTags.clear();
}
