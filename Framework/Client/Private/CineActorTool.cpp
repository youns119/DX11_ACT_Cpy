#include "pch.h"
#include "CineActorTool.h"

#include "GameInstance.h"

#include "CineDebugTool.h"

CCineActorTool::CCineActorTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCineActorTool::CCineActorTool(const CCineActorTool& Prototype)
	: super(Prototype)
{
}

HRESULT CCineActorTool::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCineActorTool::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)) ||
		FAILED(Ready_Component()) ||
		FAILED(Ready_PartObject()))
		return E_FAIL;

	return S_OK;
}

void CCineActorTool::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CCineActorTool::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	super::Update(fTimeDelta);
}

void CCineActorTool::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	_matrix matTransform = m_pTransformCom->Get_WorldMatrix();

	_vec3 vPos = _mat(matTransform).position;
	_vec3 vLook = _mat(matTransform).look;
	_vec3 vRight = _mat(matTransform).right;
	_vec3 vUp = _mat(matTransform).up;

	// DebugComponent µî·Ï
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
	m_pGameInstance->Add_DebugRender_Ray(vPos, XMVector3Normalize(vLook._vector()) * 0.2f, false, _vec4(1.f, 0.f, 0.f, 0.f));
	m_pGameInstance->Add_DebugRender_Ray(vPos, XMVector3Normalize(vRight._vector()) * 0.2f, false, _vec4(0.f, 1.f, 0.f, 0.f));
	m_pGameInstance->Add_DebugRender_Ray(vPos, XMVector3Normalize(vUp._vector()) * 0.2f, false, _vec4(0.f, 0.f, 1.f, 0.f));

#endif

	super::Late_Update(fTimeDelta);
}

void CCineActorTool::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CCineActorTool::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCineActorTool::Set_PathType(_uint iPathType)
{
	m_tCineActor.ePathType = (PATHTYPE)(iPathType);

	switch (iPathType)
	{
	case static_cast<_uint>(PATHTYPE::PATHTYPE_LINEAR):
	{
		m_pBazier->Toggle_Collider(false);
		m_pCatmullRomStart->Toggle_Collider(false);
		m_pCatmullRomEnd->Toggle_Collider(false);
	}
	break;
	case static_cast<_uint>(PATHTYPE::PATHTYPE_BAZIER):
	{
		m_pBazier->Toggle_Collider(true);
		m_pCatmullRomStart->Toggle_Collider(false);
		m_pCatmullRomEnd->Toggle_Collider(false);
	}
	break;
	case static_cast<_uint>(PATHTYPE::PATHTYPE_CATMULLROM):
	{
		m_pBazier->Toggle_Collider(false);
		m_pCatmullRomStart->Toggle_Collider(true);
		m_pCatmullRomEnd->Toggle_Collider(true);
	}
	break;
	default:
		break;
	}
}

void CCineActorTool::Set_LerpType(_uint iLerpType)
{
	m_tCineActor.eLerpType = (LERPTYPE)iLerpType;
}

void CCineActorTool::Set_BazierPos(_fvector vPos)
{
	m_tCineActor.vBazierControl = _vec3(vPos)._float3();
	m_pBazier->Set_Position(vPos);
}

void CCineActorTool::Set_CatmullRomStart(_fvector vPos)
{
	m_tCineActor.vCatmullRomControlStart = _vec3(vPos)._float3();
	m_pCatmullRomStart->Set_Position(vPos);
}

void CCineActorTool::Set_CatmullRomEnd(_fvector vPos)
{
	m_tCineActor.vCatmullRomControlEnd = _vec3(vPos)._float3();
	m_pCatmullRomEnd->Set_Position(vPos);
}

void CCineActorTool::Change_ColliderColor(_bool bChange)
{
	if (bChange)
	{
		m_pColliderCom->Set_DefaultColor(XMLoadFloat3(&m_vChangeColor));
		Set_PathType(static_cast<_uint>(m_tCineActor.ePathType));
	}
	else
	{
		m_pColliderCom->Set_DefaultColor(XMLoadFloat3(&m_vDefaultColor));
		m_pBazier->Toggle_Collider(false);
		m_pCatmullRomStart->Toggle_Collider(false);
		m_pCatmullRomEnd->Toggle_Collider(false);
	}
}

HRESULT CCineActorTool::Ready_Component()
{
	// Com_Collider
	{
		CCollider_OBB::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::DEFAULT;

		Desc.vExtents = _float3(0.2f, 0.2f, 0.2f);
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);
	}

	return S_OK;
}

HRESULT CCineActorTool::Ready_PartObject()
{
	// Bazier
	{
		CCineDebugTool::DESC tDebugDesc{};
		tDebugDesc.vPosition = m_tCineActor.vBazierControl;

		if (FAILED(Add_PartObject(L"Part_Bazier", LEVEL_STATIC, L"Prototype_GameObject_CineDebugTool", &tDebugDesc)))
			return E_FAIL;

		m_pBazier = static_cast<CCineDebugTool*>(Find_PartObject({ L"Part_Bazier" }));
	}

	// CatmullRom
	{
		CCineDebugTool::DESC tDebugDesc{};
		tDebugDesc.vPosition = m_tCineActor.vCatmullRomControlStart;

		if (FAILED(Add_PartObject(L"Part_CatmullRomStart", LEVEL_STATIC, L"Prototype_GameObject_CineDebugTool", &tDebugDesc)))
			return E_FAIL;

		m_pCatmullRomStart = static_cast<CCineDebugTool*>(Find_PartObject({ L"Part_CatmullRomStart" }));

		tDebugDesc.vPosition = m_tCineActor.vCatmullRomControlEnd;

		if (FAILED(Add_PartObject(L"Part_CatmullRomEnd", LEVEL_STATIC, L"Prototype_GameObject_CineDebugTool", &tDebugDesc)))
			return E_FAIL;

		m_pCatmullRomEnd = static_cast<CCineDebugTool*>(Find_PartObject({ L"Part_CatmullRomEnd" }));
	}

	return S_OK;
}

CCineActorTool* CCineActorTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineActorTool* pInstance = new CCineActorTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCineActorTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCineActorTool::Clone(void* pArg)
{
	return nullptr;
}

void CCineActorTool::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
}