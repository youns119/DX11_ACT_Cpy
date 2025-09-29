#include "pch.h"
#include "TriggerObserverTest.h"

#include "GameInstance.h"
#include "MapTrigger.h"

CTriggerObserverTest::CTriggerObserverTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CTriggerObserverTest::CTriggerObserverTest(const CTriggerObserverTest& Prototype)
    : super(Prototype)
{
}

HRESULT CTriggerObserverTest::Initialize_Prototype()
{
    super::Initialize_Prototype();
    return S_OK;
}

HRESULT CTriggerObserverTest::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    for (auto& strName : pDesc->TriggerNames)
    {
        /* Level::Initialize 에서 받아온 트리거 이름으로 처리 할 수도 있을 듯 */
    }

    /* 혹은 직접 이름 하드 코딩도 가능 */
    CMapTrigger* pTrigger = 
        static_cast<CMapTrigger*>(m_pGameInstance->
            Find_GameObject_With_Name(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_MapTrigger", L"Trigger_Test01"));

    //pTrigger->Add_CollisionEnter_Observer(this);


    m_FogColor = m_pGameInstance->Get_FogParams().vFogColor;

    return S_OK;
}

void CTriggerObserverTest::Priority_Update(_float fTimeDelta)
{
}

void CTriggerObserverTest::Update(_float fTimeDelta)
{
    if (true == m_isNotified)
    {
        if (m_vTimeAcc.x >= m_vTimeAcc.y)
        {
            m_isNotified = false;
            m_vTimeAcc.x = m_vTimeAcc.y;
        }
        m_vTimeAcc.x += fTimeDelta;

        FOG_PARAMS FogParams = m_pGameInstance->Get_FogParams();

        FogParams.vFogColor = _vec4::Lerp(m_FogColor, _vec4(1.f, 1.f, 1.f, 1.f), m_vTimeAcc.x / m_vTimeAcc.y);
        m_pGameInstance->Set_FogParams(FogParams);
    }

}

void CTriggerObserverTest::Late_Update(_float fTimeDelta)
{
}

void CTriggerObserverTest::On_Notify()
{
    m_isNotified = true;
    m_vTimeAcc.x = 0.f;
}


CTriggerObserverTest* CTriggerObserverTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTriggerObserverTest* pInstance = new CTriggerObserverTest(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CTriggerObserverTest");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CTriggerObserverTest::Clone(void* pArg)
{
    CTriggerObserverTest* pInstance = new CTriggerObserverTest(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CTriggerObserverTest");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTriggerObserverTest::Free()
{
    super::Free();
}
