#include "pch.h"
#include "CallbackUpdate.h"
#include "GameInstance.h"

CCallbackUpdate::CCallbackUpdate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CCallbackUpdate::CCallbackUpdate(const CCallbackUpdate& Prototype)
    : super(Prototype)
{
}

HRESULT CCallbackUpdate::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCallbackUpdate::Initialize(void* pArg)
{
    return S_OK;
}

void CCallbackUpdate::Priority_Update(_float fTimeDelta)
{
    auto iter = m_PriorityUpdateCallbacks.begin();

    while (iter != m_PriorityUpdateCallbacks.end())
    {
        auto& Pair = *iter;

        _float& fElapsed = Pair.first;
        std::function<_bool(_float, _float)>& Function = Pair.second;

        fElapsed += fTimeDelta;

        if (false == Function(fElapsed, fTimeDelta))
            ++iter;
        else
            iter = m_PriorityUpdateCallbacks.erase(iter);
    }
}

void CCallbackUpdate::Update(_float fTimeDelta)
{
    auto iter = m_UpdateCallbacks.begin();

    while (iter != m_UpdateCallbacks.end())
    {
        auto& Pair = *iter;

        _float& fElapsed = Pair.first;
        std::function<_bool(_float, _float)>& Function = Pair.second;

        fElapsed += fTimeDelta;

        if (false == Function(fElapsed, fTimeDelta))
            ++iter;
        else
            iter = m_UpdateCallbacks.erase(iter);
    }
}

void CCallbackUpdate::Late_Update(_float fTimeDelta)
{
    auto iter = m_LateUpdateCallbacks.begin();

    while (iter != m_LateUpdateCallbacks.end())
    {
        auto& Pair = *iter;

        _float& fElapsed = Pair.first;
        std::function<_bool(_float, _float)>& Function = Pair.second;

        fElapsed += fTimeDelta;

        if (false == Function(fElapsed, fTimeDelta))
            ++iter;
        else
            iter = m_LateUpdateCallbacks.erase(iter);
    }
}

HRESULT CCallbackUpdate::Render()
{
    return S_OK;
}

void CCallbackUpdate::Add_PriorityUpdate_Callback(std::function<_bool(_float fElapsed, _float fTimeDelta)> Callback)
{
    m_PriorityUpdateCallbacks.push_back({ 0.f, Callback });
}

void CCallbackUpdate::Add_Update_Callback(std::function<_bool(_float fElapsed, _float fTimeDelta)> Callback)
{
	m_UpdateCallbacks.push_back({ 0.f, Callback });
}

void CCallbackUpdate::Add_LateUpdate_Callback(std::function<_bool(_float fElapsed, _float fTimeDelta)> Callback)
{
    m_LateUpdateCallbacks.push_back({0.f, Callback});
}

CCallbackUpdate* CCallbackUpdate::Find_CallbackUpdate()
{
    return static_cast<CCallbackUpdate*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate"));
}

CCallbackUpdate* CCallbackUpdate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCallbackUpdate* pInstance = new CCallbackUpdate(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CCallbackUpdate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CCallbackUpdate::Clone(void* pArg)
{
    CCallbackUpdate* pInstance = new CCallbackUpdate(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CCallbackUpdate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCallbackUpdate::Free()
{
    super::Free();

    m_PriorityUpdateCallbacks.clear();
    m_UpdateCallbacks.clear();
    m_LateUpdateCallbacks.clear();
}
