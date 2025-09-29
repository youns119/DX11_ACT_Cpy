#include "Layer.h"
#include "GameObject.h"
#include "ContainerObject.h"

#include "GameInstance.h"

CLayer::CLayer()
{
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		if (true == pGameObject->Get_IsActive())
		{
			if (pGameObject->Get_IsLag())
				pGameObject->Priority_Update(fTimeDelta);
			else pGameObject->Priority_Update(fTimeDelta / CGameInstance::GetInstance()->Get_LagRatio());
		}
			
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		if (true == pGameObject->Get_IsActive())
		{
			if (pGameObject->Get_IsLag())
				pGameObject->Update(fTimeDelta);
			else pGameObject->Update(fTimeDelta / CGameInstance::GetInstance()->Get_LagRatio());
		}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		if (true == pGameObject->Get_IsActive())
		{
			if (pGameObject->Get_IsLag())
				pGameObject->Late_Update(fTimeDelta);
			else pGameObject->Late_Update(fTimeDelta / CGameInstance::GetInstance()->Get_LagRatio());
		}
}

void CLayer::Final_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		if (true == pGameObject->Get_IsActive())
		{
			if (pGameObject->Get_IsLag())
				pGameObject->Final_Update(fTimeDelta);
			else pGameObject->Final_Update(fTimeDelta / CGameInstance::GetInstance()->Get_LagRatio());
		}
}

CComponent* CLayer::Find_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag);
}

CComponent* CLayer::Find_Component(const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	//return dynamic_cast<CContainerObject*>(*iter)->Find_Component_In_PartObject(strPartTag, strComponentTag);
	//return (*iter)->Find_Component_In_PartObject(strPartTag, strComponentTag);
	//MSG_BOX("this Function will be destroy");
	//__debugbreak();

	vector<_wstring> PartTags;
	PartTags.push_back(strPartTag);
	return (*iter)->Find_Component_In_PartObject(PartTags, strComponentTag);
}

CComponent* CLayer::Find_Component_In_PartObject(const vector<_wstring>& strPartTags, const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component_In_PartObject(strPartTags, strComponentTag);
}

void CLayer::Delete_GameObject(_uint iIndex)
{
	auto iter = m_GameObjects.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	if (m_GameObjects.end() == iter)
		return;

	(*iter)->Set_IsDead(); // 삭제를 하는데 해당 프레임의 collision manager 에서 퇴출당함
	Safe_Release(*iter);
	m_GameObjects.erase(iter);
}

void CLayer::Delete_GameObject(CGameObject* pGameObject)
{
	auto iter = m_GameObjects.begin();
	for (auto& pObject : m_GameObjects)
	{
		if (pObject != pGameObject)
			iter++;
		else
			break;
	}

	if (m_GameObjects.end() == iter)
		return;

	(*iter)->Set_IsDead();
	Safe_Release(*iter);
	m_GameObjects.erase(iter);
}

void CLayer::Delete_GameObject(const vector<_wstring>& strPartTags, _uint iIndex)
{
	auto iter = m_GameObjects.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;
	
	(*iter)->Delete_PartObject(strPartTags);
}

CGameObject* CLayer::Find_GameObject(_uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return *iter;
}

CGameObject* CLayer::Find_GameObject(const _wstring& strName)
{
	auto iter = m_GameObjects.begin();

	while (iter != m_GameObjects.end())
	{
		if ((*iter)->Get_Name() == strName)
			return *iter;
		++iter;
	}

	return nullptr; 
}


CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	CBase::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
