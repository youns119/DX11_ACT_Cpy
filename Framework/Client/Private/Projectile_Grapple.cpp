#include "pch.h"
#include "Projectile_Grapple.h"

#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "Movement.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "CineCamera_Player.h"
#include "Shell_World.h"

#include "Effect_Textured_Trail.h"

CProjectile_Grapple::CProjectile_Grapple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CProjectile_Grapple::CProjectile_Grapple(const CProjectile_Grapple& Prototype)
	: super(Prototype)
{
}

HRESULT CProjectile_Grapple::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_Grapple::Initialize(void* pArg)
{
	CProjectile::DESC tDesc{};

	if (FAILED(super::Initialize(&tDesc)) ||
		FAILED(Ready_Components()) ||
		FAILED(Ready_PartObject()))
		return E_FAIL;

	Deactivate();

	return S_OK;
}

void CProjectile_Grapple::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CProjectile_Grapple::Update(_float fTimeDelta)
{
	if (m_bBazier)
	{
		m_fBazierTime += fTimeDelta / m_fDuration;

		_vec3 vPos = (1 - m_fBazierTime) * (1 - m_fBazierTime) * _vec3(m_vSrc) +
			2 * (1 - m_fBazierTime) * m_fBazierTime * _vec3(m_vMid) +
			m_fBazierTime * m_fBazierTime * _vec3(m_vDst);

		m_pTransformCom->Set_State(CTransform::STATE::POSITION, vPos._vector());

		_vec3 vLookDir = vPos - _vec3(m_vPrevPos);
		m_pTransformCom->UpAt((vPos + vLookDir)._vector());
	}

	_vec3 vPrevPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	m_pMovementCom->Update(fTimeDelta);

	if (!m_bBazier)
	{
		_vec3 vCurrPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vec3 vLookDir = vCurrPos - vPrevPos;

		m_pTransformCom->UpAt((vCurrPos + vLookDir)._vector());
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CProjectile_Grapple::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

	super::Late_Update(fTimeDelta);
}

void CProjectile_Grapple::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CProjectile_Grapple::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iShaderPass = 0;
		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

			if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
			return E_FAIL;

		m_pShaderCom->Begin(iShaderPass);
		m_pModelCom->Render(i);
	}

	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CProjectile_Grapple::Activate(const FProjectileStat& tProjectileStat)
{
	super::Activate(tProjectileStat);

	m_pTransformCom->Locate(_vec3(tProjectileStat.vSrc)._float3());
	m_pMovementCom->Set_Decel(tProjectileStat.fDecel);
	m_pMovementCom->Set_MaxSpeed(m_tProjectileStat.fForce);

	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
	m_pTarget = m_pPlayer->Get_GrappleTarget();
	_bool bGrappable = m_pPlayer->Get_PlayerStat()->bGrapplable;
	if (m_pTarget != nullptr && bGrappable) m_bHasTarget = true;

	_vector vDir = (_vec3(tProjectileStat.vDst) - _vec3(tProjectileStat.vSrc))._vector();
	_float fGrappleDist = m_pPlayer->Get_PlayerStat()->fGrappleDist;
	_float fTargetDist = _vec3(vDir).Length();
	m_fDuration = fTargetDist / m_tProjectileStat.fForce;

	if (m_bHasTarget)
	{
		m_pMovementCom->Set_ForceFriction(0.f);

		if (fTargetDist >= fGrappleDist * 0.5f)
		{
			m_bBazier = true;

			_vec3 vSrc = m_vPrevPos = tProjectileStat.vSrc;
			_vec3 vDst = tProjectileStat.vDst;
			_vec3 vMid = vSrc + (vDst - vSrc) * 0.6f + _vec3(0.f, 5.f, 0.f);

			m_vSrc = vSrc._float3();
			m_vDst = vDst._float3();
			m_vMid = vMid._float3();
		}
		else
		{
			m_bBazier = false;
			m_pMovementCom->Add_Force(vDir, m_tProjectileStat.fForce);
		}
	}
	else
	{
		m_pMovementCom->FlyMode(false);
		m_pMovementCom->Set_ForceFriction(0.f);

		_vec3 vFinalDir = _vec3(vDir);
		vFinalDir = vFinalDir.Normalized() + _vec3(0.f, 0.3f, 0.f);

		_vec3 vCurrPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		m_pTransformCom->LookAt((vCurrPos + vFinalDir)._vector());
		m_pMovementCom->Add_Force(vFinalDir._vector(), m_tProjectileStat.fForce);
	}

	m_pPlayer->Play_PlayerSound(L"Grapple_Throw");
}

void CProjectile_Grapple::Deactivate()
{
	super::Deactivate();

	m_pPlayer = nullptr;
	m_pTarget = nullptr;

	m_bHasTarget = false;

	m_bBazier = false;
	m_fBazierTime = 0.f;
	m_fDuration = 0.f;
	m_vSrc = m_vDst = m_vMid = _float3(0.f, 0.f, 0.f);

	m_pMovementCom->Reset_Velocity();
	m_pMovementCom->FlyMode(true);

	static_cast<CEffect_Textured_Trail*>(m_PartObjects.find(L"Part_Trail")->second)->Clear_Info();
}

void CProjectile_Grapple::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	if (pOther != m_pTarget)
		return;

	switch (iGroup)
	{
	case (_uint)COLLISION_GROUP::MONSTER:
	{
		m_pPlayer->Grapple_Hit();
	}
	break;
	case (_uint)COLLISION_GROUP::GRAPPLENEEDLE:
	{
		m_pPlayer->Grapple_Hit();
	}
	break;
	case(_uint)COLLISION_GROUP::SHELL :
	{
		m_pPlayer->Grapple_Hit();
		CCollider* pCollider = static_cast<CCollider*>(m_pPlayer->Find_Component(L"Com_Collider"));
		CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
		//_vec3 vPos = pTransform->Get_State(CTransform::STATE::POSITION);
		_vec3 vPos = pCollider->Get_BoundingCenter();
		vPos.y -= 0.3f;
		_vec3 vLook = pTransform->Get_State(CTransform::STATE::LOOK);

		CShell_World* pShell = static_cast<CShell_World*>(pOther);
		IGrapple* pGrapple = dynamic_cast<IGrapple*>(pOther);
		if (pGrapple == nullptr)
			return;

		_vec3 vSrc = pGrapple->Get_GrapplePos();
		_vec3 vDst = vPos + vLook;
		pShell->Shell_Launch(vSrc, vDst);
	}
	break;
	default :
		break;
	}

	Deactivate();
}

void CProjectile_Grapple::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CProjectile_Grapple::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CProjectile_Grapple::Ready_Components()
{
	// Com_Model
	if (FAILED(Add_Component
	(
		LEVEL_STATIC,
		L"Prototype_Component_Model_Fork",
		L"Com_Model",
		reinterpret_cast<CComponent**>(&m_pModelCom)
	)))
		return E_FAIL;

	// Com_Shader
	if (FAILED(super::Add_Component
	(
		LEVEL_STATIC,
		TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom)
	)))
		return E_FAIL;

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::PLAYER_GRAPPLE;

		Desc.fRadius = 0.3f;
		Desc.vCenter = { 0.f, 0.3f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}

	// Com_Movement
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 40.f;
		Desc.fMoveDecel = 20.f;
		Desc.fMinSpeed = 1.f;
		Desc.fMaxSpeed = 10.f;
		Desc.fForceFriction = 0.f;
		Desc.fRotationScale = 1.f;
		Desc.fGravityScale = 3.f;
		Desc.fMaxFallingSpeed = 5.f;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
			L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &Desc)))
			return E_FAIL;

		m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
		m_pMovementCom->Set_RotateToDirection_3D(true);
		m_pMovementCom->FlyMode(true);
	}

	return S_OK;
}

HRESULT CProjectile_Grapple::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ref())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_Grapple::Ready_PartObject()
{
	CGameObject::GAMEOBJECT_DESC PartDesc{};
	PartDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
	if (FAILED(Add_PartObject(L"Part_Trail", LEVEL_STATIC, L"Prototype_GameObject_Effect_TexturedTrail", &PartDesc)))
		return E_FAIL;

	return S_OK;
}

CProjectile_Grapple* CProjectile_Grapple::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Grapple* pInstance = new CProjectile_Grapple(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CProjectile_Grapple");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CProjectile_Grapple::Clone(void* pArg)
{
	CProjectile_Grapple* pInstance = new CProjectile_Grapple(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CProjectile_Grapple");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Grapple::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}