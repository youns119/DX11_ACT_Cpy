#include "pch.h"
#include "CineDebugTool.h"

#include "GameInstance.h"

CCineDebugTool::CCineDebugTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCineDebugTool::CCineDebugTool(const CCineDebugTool& Prototype)
	: super(Prototype)
{
}

HRESULT CCineDebugTool::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCineDebugTool::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)) ||
		FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Locate(pDesc->vPosition);

	return S_OK;
}

void CCineDebugTool::Priority_Update(_float fTimeDelta)
{
}

void CCineDebugTool::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CCineDebugTool::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	_matrix matTransform = m_pTransformCom->Get_WorldMatrix();

	// DebugComponent µî·Ï
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif
}

void CCineDebugTool::Final_Update(_float fTimeDelta)
{
}

HRESULT CCineDebugTool::Render()
{
	return S_OK;
}

void CCineDebugTool::Set_Position(_fvector vPos)
{
	m_pTransformCom->Locate(_vec3(vPos)._float3());
}

void CCineDebugTool::Toggle_Collider(_bool bToggle)
{
	m_pColliderCom->Set_Active(bToggle);
}

HRESULT CCineDebugTool::Ready_Component()
{
	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::DEFAULT;

		Desc.fRadius = 0.1f;
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
		m_pColliderCom->Set_DefaultColor(XMVectorSet(1.f, 0.f, 1.f, 1.f));
	}

	return S_OK;
}

CCineDebugTool* CCineDebugTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineDebugTool* pInstance = new CCineDebugTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCineDebugTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCineDebugTool::Clone(void* pArg)
{
	CCineDebugTool* pInstance = new CCineDebugTool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCineDebugTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCineDebugTool::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
}