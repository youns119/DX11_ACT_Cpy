#include "pch.h"
#include "MapTrigger.h"

#include "GameInstance.h"
#include "Observer.h"
#include "CallbackUpdate.h"

#include "Theme.h"

CMapTrigger::CMapTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CMapTrigger::CMapTrigger(const CMapTrigger& Prototype)
    : super(Prototype)
{
}

HRESULT CMapTrigger::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapTrigger::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMapTrigger::Priority_Update(_float fTimeDelta)
{
}

void CMapTrigger::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMapTrigger::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
    m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
    _mat WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    WorldMatrix.position.y += WorldMatrix.up.Length() * 0.5f;
    m_pGameInstance->Add_DebugRender_Box(m_pTransformCom->Get_WorldMatrix(), _vec4(1.f, 0.f, 1.f, 1.f));
#endif
}

HRESULT CMapTrigger::Render()
{
    return S_OK;
}

void CMapTrigger::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
    for (auto& Callback : m_CollisionEnterCallbacks)
        Callback();

    for (auto& pObserver : m_CollisionEnterObservers)
        pObserver->On_Notify();
}

void CMapTrigger::On_Collision(CGameObject* pOther, _uint iGroup)
{
    for (auto& Callback : m_CollisionCallbacks)
        Callback();

    for (auto& pObserver : m_CollisionObservers)
        pObserver->On_Notify();
}

void CMapTrigger::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
    for (auto& Callback : m_CollisionExitCallbacks)
        Callback();

    for (auto& pObserver : m_CollisionExitObservers)
        pObserver->On_Notify();
}

void CMapTrigger::Add_CollisionEnter_Callback(std::function<void()> Callback)
{
    m_CollisionEnterCallbacks.push_back(Callback);
}

void CMapTrigger::Add_Collision_Callback(std::function<void()> Callback)
{
    m_CollisionCallbacks.push_back(Callback);
}

void CMapTrigger::Add_CollisionExit_Callback(std::function<void()> Callback)
{
    m_CollisionExitCallbacks.push_back(Callback);
}

void CMapTrigger::Add_CollisionEnter_Observer(IObserver* pObserver)
{
    m_CollisionEnterObservers.push_back(pObserver);
}

void CMapTrigger::Add_Collision_Observer(IObserver* pObserver)
{
    m_CollisionObservers.push_back(pObserver);
}

void CMapTrigger::Add_CollisionExit_Observer(IObserver* pObserver)
{
    m_CollisionExitObservers.push_back(pObserver);
}

void CMapTrigger::Resist_MapTrigger_CallingEffect(LEVELID iLevelID, const _wstring& strTriggerName, _uint iEffectType, _fvector vPos, _fvector vLook)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();

    CMapTrigger* pTrigger = static_cast<CMapTrigger*>(pGameInstance->
        Find_GameObject_With_Name(iLevelID, L"Layer_MapTrigger", strTriggerName));

    pTrigger->Add_CollisionEnter_Callback([iEffectType, vPos, vLook]()->void
        {
            CGameInstance* pGameInstance = CGameInstance::GetInstance();
            CCallbackUpdate* pCallbackUpdate =
                static_cast<CCallbackUpdate*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate", 0));

            pGameInstance->Call_Effect(iEffectType, vPos, vLook, false, nullptr);
        });
}

void CMapTrigger::Resist_MapTrigger_ThemeChanger(LEVELID iLevelID, const _wstring& strTriggerName, const THEME_DESC* pDst, _float fDuration)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();

    CMapTrigger* pTrigger = static_cast<CMapTrigger*>(pGameInstance->
        Find_GameObject_With_Name(iLevelID, L"Layer_MapTrigger", strTriggerName));

    pTrigger->Add_CollisionEnter_Callback([pDst]()->void
        {
            THEME_DESC SrcTheme = THEME_DESC::Get_CurrentAtmospehre();

            CCallbackUpdate* pCallbackUpdate =
                static_cast<CCallbackUpdate*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_CallbackUpdate", 0));

            pCallbackUpdate->Add_Update_Callback([SrcTheme, pDst](_float fElapsed, _float fTimeDelta)->_bool
                {
                    _float fDuration = 1.f;
                    THEME_DESC Desc = THEME_DESC::Lerp(SrcTheme, *pDst, min(fElapsed / fDuration, 1.f));

                    CGameInstance* pGameInstance = CGameInstance::GetInstance();
                    pGameInstance->Set_LightDesc(L"Light_Global", Desc.LightDesc);
                    pGameInstance->SetUp_ShadowLight(_vec3(Desc.LightDesc.vDirection));
                    pGameInstance->Set_ColorCorrectionParams(Desc.ColorCorrectionParams);
                    pGameInstance->Set_BloomParams(Desc.BloomParams);
                    pGameInstance->Set_FogParams(Desc.FogParams);

                    return fElapsed >= fDuration;
                });
        });
}

void CMapTrigger::Resist_MapTrigger_SoundChanger(LEVELID iLevelID, const _wstring& strTriggerName, const _wstring& strSoundTag, _float fSwapTime, _float fVolume)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CMapTrigger* pTrigger = static_cast<CMapTrigger*>(pGameInstance->
		Find_GameObject_With_Name(iLevelID, L"Layer_MapTrigger", strTriggerName));

	pTrigger->Add_CollisionEnter_Callback([strSoundTag, fSwapTime, fVolume]()->void
		{
            CGameInstance::GetInstance()->FadeSwap_Sound(strSoundTag, (_uint)SOUND_CHANNEL::BGM, fSwapTime, fVolume);
		});
}

HRESULT CMapTrigger::Create_InversedMapTrigger(LEVELID iLevelID, const _wstring& strTriggerName, const _wstring& strInverseTriggerName, _vec3 vOffset)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    CMapTrigger* pTrigger = static_cast<CMapTrigger*>(pGameInstance->Find_GameObject_With_Name(iLevelID, L"Layer_MapTrigger", strTriggerName));

	/* 원복할 맵트리거생성 */
	CMapTrigger::DESC Desc {};
    {
		Desc.GameObjectDesc.strName = strInverseTriggerName;
		_mat WorldMatrix = static_cast<CTransform*>(pTrigger->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
		WorldMatrix.position += vOffset;
		Desc.GameObjectDesc.WorldMatrix = WorldMatrix._float4x4();
    }
	if (FAILED(pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_MapTrigger", iLevelID, L"Layer_MapTrigger", &Desc)))
		return E_FAIL;

    CMapTrigger* pInverseTrigger = static_cast<CMapTrigger*>(pGameInstance->Find_GameObject_With_Name(iLevelID, L"Layer_MapTrigger", strInverseTriggerName));
    pInverseTrigger->Set_IsActive(false);

	/* 해당 맵트리거 콜라이더에서 나올때 자신은 비활성, 다시 원복할 맵트리거 활성화 */
	pTrigger->Add_CollisionExit_Callback([iLevelID, strInverseTriggerName, pTrigger, pInverseTrigger]()->void
		{
            pInverseTrigger->Set_IsActive(true);
			static_cast<CCollider*>(pInverseTrigger->Find_Component(L"Com_Collider"))->Set_Active(true);
			pTrigger->Set_IsActive(false);
			static_cast<CCollider*>(pTrigger->Find_Component(L"Com_Collider"))->Set_Active(false);
		});


	/* 해당 맵트리거 콜라이더에서 나올때 자신은 비활성, 다시 원복할 맵트리거 활성화 */
    pInverseTrigger->Add_CollisionExit_Callback([iLevelID, pTrigger, pInverseTrigger]()->void
		{
            pTrigger->Set_IsActive(true);
			static_cast<CCollider*>(pTrigger->Find_Component(L"Com_Collider"))->Set_Active(true);
            pInverseTrigger->Set_IsActive(false);
			static_cast<CCollider*>(pInverseTrigger->Find_Component(L"Com_Collider"))->Set_Active(false);
		});

    return S_OK;
}

HRESULT CMapTrigger::Ready_Components()
{
    /* Com_Collider */
    {
        CCollider_OBB::DESC Desc{};
        Desc.ColliderDesc.pOwner = this;
        Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::TRIGGER);

        //_vec3 vScale = m_pTransformCom->Get_Scale();
        //Desc.vExtents = _vec3(vScale * 0.5f)._float3();
        Desc.vExtents = _vec3(_vec3(1.f, 1.f, 1.f) * 0.5f)._float3();
        //Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };
        Desc.vCenter = _float3{};
        Desc.vRotation = { 0.f, 0.f, 0.f };

        if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
            L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

CMapTrigger* CMapTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapTrigger* pInstance = new CMapTrigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CMapTrigger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CMapTrigger::Clone(void* pArg)
{
    CMapTrigger* pInstance = new CMapTrigger(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CMapTrigger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapTrigger::Free()
{
    super::Free();

    Safe_Release(m_pColliderCom);
}
