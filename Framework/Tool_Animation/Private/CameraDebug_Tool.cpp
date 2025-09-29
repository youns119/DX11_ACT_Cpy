#include "pch.h"
#include "CameraDebug_Tool.h"

#include "GameInstance.h"

CCameraDebug_Tool::CCameraDebug_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCameraDebug_Tool::CCameraDebug_Tool(const CCameraDebug_Tool& Prototype)
	: super(Prototype)
{
}

HRESULT CCameraDebug_Tool::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraDebug_Tool::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)) ||
		FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Locate(pDesc->vPosition);

	return S_OK;
}

void CCameraDebug_Tool::Priority_Update(_float fTimeDelta)
{
}

void CCameraDebug_Tool::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CCameraDebug_Tool::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	_matrix matTransform = m_pTransformCom->Get_WorldMatrix();

	// DebugComponent µî·Ï
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif
}

void CCameraDebug_Tool::Final_Update(_float fTimeDelta)
{
}

HRESULT CCameraDebug_Tool::Render()
{
	return S_OK;
}

void CCameraDebug_Tool::Set_Position(_fvector vPos)
{
	m_pTransformCom->Locate(_vec3(vPos)._float3());
}

void CCameraDebug_Tool::Toggle_Collider(_bool bToggle)
{
	m_pColliderCom->Set_Active(bToggle);
}

HRESULT CCameraDebug_Tool::Ready_Component()
{
	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::CAMERADEBUG;

		Desc.fRadius = 0.1f;
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Collider_Sphere",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
		m_pColliderCom->Set_DefaultColor(XMVectorSet(1.f, 0.f, 1.f, 1.f));
	}

	return S_OK;
}

CCameraDebug_Tool* CCameraDebug_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraDebug_Tool* pInstance = new CCameraDebug_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCameraDebug_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCameraDebug_Tool::Clone(void* pArg)
{
	CCameraDebug_Tool* pInstance = new CCameraDebug_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCameraDebug_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraDebug_Tool::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
}