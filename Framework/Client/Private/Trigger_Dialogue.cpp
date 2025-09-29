#include "pch.h"
#include "Trigger_Dialogue.h"

#include "GameInstance.h"
#include "UI_Dialogue.h"

CTrigger_Dialogue::CTrigger_Dialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CTrigger_Dialogue::CTrigger_Dialogue(const CTrigger_Dialogue& Prototype)
	: super(Prototype)
{
}

HRESULT CTrigger_Dialogue::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger_Dialogue::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;
	
	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	return S_OK;
}

void CTrigger_Dialogue::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CTrigger_Dialogue::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CTrigger_Dialogue::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

	super::Late_Update(fTimeDelta);
}

HRESULT CTrigger_Dialogue::Render()
{
	return S_OK;
}

void CTrigger_Dialogue::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
}

void CTrigger_Dialogue::On_Collision(CGameObject* pOther, _uint iGroup)
{

}

void CTrigger_Dialogue::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CTrigger_Dialogue::Add_CollisionEnter_Callback(std::function<void()> Callback)
{
}

HRESULT CTrigger_Dialogue::Ready_Components(DESC& triggerDesc)
{
	/* Com_Collider */
	{
		CCollider_AABB::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)(COLLISION_GROUP::TRIGGER);

		Desc.vExtents = triggerDesc.vExtents;
		Desc.vCenter = { 0.f, Desc.vExtents.y, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_AABB",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}

	/* Dialogue */
	CUI_Dialogue::DESC DialogueDesc{};
	{
	}
	if (FAILED(Add_PartObject(L"UI_Dialogue", LEVEL_STATIC, L"Prototype_GameObject_UI_Dialogue", &DialogueDesc)))
		return E_FAIL;

	return S_OK;
}


CTrigger_Dialogue* CTrigger_Dialogue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrigger_Dialogue* pInstance = new CTrigger_Dialogue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CTrigger_Dialogue_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CTrigger_Dialogue::Clone(void* pArg)
{
	CTrigger_Dialogue* pInstance = new CTrigger_Dialogue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CTrigger_Dialogue_Dialogue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger_Dialogue::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
}
