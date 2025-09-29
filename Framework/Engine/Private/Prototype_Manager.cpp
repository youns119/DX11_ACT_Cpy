#include "Prototype_Manager.h"

#include "GameObject.h"
#include "Cloneable.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
    m_iNumLevels = iNumLevels;

    m_Prototypes.resize(iNumLevels);

    return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CCloneable* pPrototype)
{
    if (iLevelIndex >= m_iNumLevels ||
        nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
        return E_FAIL;

    m_Prototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

    return S_OK;
}

//CBase* CPrototype_Manager::Clone_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
//{
//    CBase* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
//    if (nullptr == pPrototype)
//        return nullptr;
//
//    // 삼항 연산자 혹은 switch case 문으로 해야함 원래는
//    return dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
//}

CCloneable* CPrototype_Manager::Clone_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
    CCloneable* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);

    if (nullptr == pPrototype)
        return nullptr;

    return pPrototype->Clone(pArg);

    // 삼항 연산자 혹은 switch case 문으로 해야함 원래는
    //return dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
}

void CPrototype_Manager::Clear(_uint iLevelIndex)
{
    for (auto& Pair : m_Prototypes[iLevelIndex])
        Safe_Release(Pair.second);

    m_Prototypes[iLevelIndex].clear();
}

vector<_wstring> CPrototype_Manager::Get_PrototypeTags(_uint iLevelIndex)
{
    vector<_wstring> PrototypeTags;
    PrototypeTags.reserve(m_Prototypes[iLevelIndex].size());

    for (auto& Pair : m_Prototypes[iLevelIndex])
    {
        PrototypeTags.push_back(Pair.first);
    }

    return PrototypeTags;
}

CCloneable* CPrototype_Manager::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
    auto iter = m_Prototypes[iLevelIndex].find(strPrototypeTag);

    if (iter == m_Prototypes[iLevelIndex].end())
        return nullptr;

    return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
    CPrototype_Manager* pInstance = new CPrototype_Manager();

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Create : CPrototype_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPrototype_Manager::Free()
{
    CBase::Free();

    for (auto& Prototypes : m_Prototypes)
    {
        for (auto& Pair : Prototypes)
            Safe_Release(Pair.second);
        Prototypes.clear();
    }
    m_Prototypes.clear();
}
