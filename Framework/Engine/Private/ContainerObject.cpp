#include "ContainerObject.h"

#include "PartObject.h"
#include "GameInstance.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CContainerObject::CContainerObject(const CContainerObject& Prototype)
	: super(Prototype)
	// part object 를 추가하는건 initialize 단계 이므로 복사 생성 시점에 m_PartObjects는 비어있음
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	CGameObject::GAMEOBJECT_DESC* pGameObjectDesc { nullptr };

	if (nullptr != pDesc)
	{
		pGameObjectDesc = &pDesc->GameObjectDesc;
	}

	if (FAILED(super::Initialize(pGameObjectDesc)))
		return E_FAIL;


	return S_OK;
}

void CContainerObject::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Update(fTimeDelta);
	}

}

void CContainerObject::Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Update(fTimeDelta);
	}

}

void CContainerObject::Late_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Update(fTimeDelta);
	}

}

HRESULT CContainerObject::Render()
{
	return S_OK;
}

CComponent* CContainerObject::Find_Component_In_PartObject(const _wstring& strPartTag, const _wstring& strComponentTag)
{
	auto iter = m_PartObjects.find(strPartTag);
	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second->Find_Component(strComponentTag);
}


HRESULT CContainerObject::Add_PartObject(const _wstring& strPartTag, _uint iPrototypeLevelID, const _wstring& strPrototypeTag, void* pArg)
{
	CPartObject* pPartObject = nullptr;

	pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(iPrototypeLevelID, strPrototypeTag, pArg));
	if (nullptr == pPartObject)
		return E_FAIL;


	m_PartObjects.emplace(strPartTag, pPartObject);
;

	return S_OK;
}

CPartObject* CContainerObject::Find_PartObject(const _wstring& strPartTag)
{
	auto iter = m_PartObjects.find(strPartTag);
	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;
}

void CContainerObject::Free()
{
	super::Free();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

	m_PartObjects.clear();
}
