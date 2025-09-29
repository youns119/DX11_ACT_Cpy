#include "pch.h"
#include "CineActor_Tool.h"

#include "GameInstance.h"

#include "CameraDebug_Tool.h"

CCineActor_Tool::CCineActor_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCineActor_Tool::CCineActor_Tool(const CCineActor_Tool& Prototype)
	: super(Prototype)
{
}

HRESULT CCineActor_Tool::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCineActor_Tool::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_tCineActor.ePathType = pDesc->tCineActor.ePathType;
	m_tCineActor.fTime = pDesc->tCineActor.fTime;
	m_tCineActor.fLerp = pDesc->tCineActor.fLerp;

	m_tCineActor.eLerpType = pDesc->tCineActor.eLerpType;

	m_tCineActor.vBazierControl = pDesc->tCineActor.vBazierControl;

	m_tCineActor.vCatmullRomControlStart = pDesc->tCineActor.vCatmullRomControlStart;
	m_tCineActor.vCatmullRomControlEnd = pDesc->tCineActor.vCatmullRomControlEnd;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)) ||
		FAILED(Ready_Component()) ||
		FAILED(Ready_PartObject()))
		return E_FAIL;

	m_pTransformCom->Locate(pDesc->vPosition);
	m_pTransformCom->Rotate(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

	return S_OK;
}

void CCineActor_Tool::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CCineActor_Tool::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	super::Update(fTimeDelta);
}

void CCineActor_Tool::Late_Update(_float fTimeDelta)
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

void CCineActor_Tool::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CCineActor_Tool::Render()
{
	return S_OK;
}

void CCineActor_Tool::Set_PathType(_uint iPathType)
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
	case static_cast<_uint>(PATHTYPE::PATHTYPE_BAZIER) :
	{
		m_pBazier->Toggle_Collider(true);
		m_pCatmullRomStart->Toggle_Collider(false);
		m_pCatmullRomEnd->Toggle_Collider(false);
	}
	break;
	case static_cast<_uint>(PATHTYPE::PATHTYPE_CATMULLROM) :
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

void CCineActor_Tool::Set_LerpType(_uint iLerpType)
{
	m_tCineActor.eLerpType = (LERPTYPE)iLerpType;
}

void CCineActor_Tool::Set_BazierPos(_fvector vPos)
{
	m_tCineActor.vBazierControl = _vec3(vPos)._float3();
	m_pBazier->Set_Position(vPos);
}

void CCineActor_Tool::Set_CatmullRomStart(_fvector vPos)
{
	m_tCineActor.vCatmullRomControlStart = _vec3(vPos)._float3();
	m_pCatmullRomStart->Set_Position(vPos);
}

void CCineActor_Tool::Set_CatmullRomEnd(_fvector vPos)
{
	m_tCineActor.vCatmullRomControlEnd = _vec3(vPos)._float3();
	m_pCatmullRomEnd->Set_Position(vPos);
}

void CCineActor_Tool::Change_ColliderColor(_bool bChange)
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

HRESULT CCineActor_Tool::Ready_Component()
{
	// Com_Collider
	{
		CCollider_OBB::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::CAMERADEBUG;

		Desc.vExtents = _float3(0.1f, 0.1f, 0.1f);
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Collider_OBB",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);
	}

	return S_OK;
}

HRESULT CCineActor_Tool::Ready_PartObject()
{
	// Bazier
	{
		CCameraDebug_Tool::DESC tDebugDesc{};
		tDebugDesc.vPosition = m_tCineActor.vBazierControl;

		if (FAILED(Add_PartObject(L"Part_Bazier", LEVEL_TOOL, L"GameObject_CameraDebug_Tool", &tDebugDesc)))
			return E_FAIL;

		m_pBazier = static_cast<CCameraDebug_Tool*>(Find_PartObject({ L"Part_Bazier" }));
	}

	// CatmullRom
	{
		CCameraDebug_Tool::DESC tDebugDesc{};
		tDebugDesc.vPosition = m_tCineActor.vCatmullRomControlStart;

		if (FAILED(Add_PartObject(L"Part_CatmullRomStart", LEVEL_TOOL, L"GameObject_CameraDebug_Tool", &tDebugDesc)))
			return E_FAIL;

		m_pCatmullRomStart = static_cast<CCameraDebug_Tool*>(Find_PartObject({ L"Part_CatmullRomStart" }));

		tDebugDesc.vPosition = m_tCineActor.vCatmullRomControlEnd;

		if (FAILED(Add_PartObject(L"Part_CatmullRomEnd", LEVEL_TOOL, L"GameObject_CameraDebug_Tool", &tDebugDesc)))
			return E_FAIL;

		m_pCatmullRomEnd = static_cast<CCameraDebug_Tool*>(Find_PartObject({ L"Part_CatmullRomEnd" }));
	}

	return S_OK;
}

CCineActor_Tool* CCineActor_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineActor_Tool* pInstance = new CCineActor_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCineActor_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCineActor_Tool::Clone(void* pArg)
{
	return nullptr;
}

void CCineActor_Tool::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
}