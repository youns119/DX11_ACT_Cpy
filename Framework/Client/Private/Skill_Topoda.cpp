#include "pch.h"
#include "Skill_Topoda.h"

#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "Shader.h"
#include "Collider.h"
#include "Movement.h"

#include "Player.h"
#include "SpringArm_Player.h"

#include "Event_MantisPunch_ColliderOn.h"
#include "Event_MantisPunch_ColliderOff.h"
#include "Event_MantisPunch_Dettach.h"
#include "Event_MantisPunch_Retreat.h"
#include "Event_Effect_MantisPunch_Topoda.h"

CSkill_Topoda::CSkill_Topoda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CSkill_Topoda::CSkill_Topoda(const CSkill_Topoda& Prototype)
	: super(Prototype)
{
}

HRESULT CSkill_Topoda::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Topoda::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	m_pOwner = pDesc->pOwner;

	if (FAILED(super::Initialize(&pDesc->tGameObjectDesc)) ||
		FAILED(Ready_Component()) ||
		FAILED(Ready_Event()))
		return E_FAIL;

	m_iAnimIndex[static_cast<_uint>(TOPODA::TOPODA_PUNCH)] = m_pModelCom->Get_AnimationIndex("MantisPunchSandstorm");
	m_iAnimIndex[static_cast<_uint>(TOPODA::TOPODA_RETREAT)] = m_pModelCom->Get_AnimationIndex("SandRetreat");

	m_pColliderCom->Set_Active(false);

	return S_OK;
}

void CSkill_Topoda::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bAttach)
	{
		m_fLerpAcc += fTimeDelta;
		if (m_fLerpAcc >= m_fLerpTime)
			m_fLerpAcc = m_fLerpTime;

		_vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
		_float fDist = 100 * sinf(XMConvertToRadians(90.f) * (m_fLerpAcc / m_fLerpTime));

		vLook = vLook * (-100.f + fDist);
		m_pTransformCom->Locate(vLook._float3());
	}

	super::Priority_Update(fTimeDelta);
}

void CSkill_Topoda::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_pModelCom->Get_CurrAnimation()->Get_Blendable())
	{
		if (m_eTopoda == TOPODA::TOPODA_PUNCH)
		{
			m_eTopoda = TOPODA::TOPODA_RETREAT;
			m_pModelCom->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eTopoda)], false, 0.1f);
		}
		else if (m_eTopoda == TOPODA::TOPODA_RETREAT)
		{
			Despawn();
			return;
		}
	}

	// Model
	m_pModelCom->Play_Animation(fTimeDelta);

	// Movement
	m_pMovementCom->Update(fTimeDelta);

	// Collider
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	super::Update(fTimeDelta);
}

void CSkill_Topoda::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	// Renderer 등록 및 Physics 등록
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	// DebugComponent 등록
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif

	m_pMovementCom->Late_Update(fTimeDelta);

	super::Late_Update(fTimeDelta);
}

void CSkill_Topoda::Final_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bAttach)
	{
		_matrix SocketMatrix = XMLoadFloat4x4(&m_matSocket);

		for (size_t i = 0; i < 3; ++i)
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
	}
	else
		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	super::Final_Update(fTimeDelta);
}

HRESULT CSkill_Topoda::Render()
{
	if (!m_bActive)
		return S_OK;

	if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	m_vCamPosition = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_vCamPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		_uint iShaderPass = 0;
		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::RIMLIGHT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR;

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

	return S_OK;
}

void CSkill_Topoda::Spawn()
{
	m_bActive = true;
	m_bAttach = true;

	m_fLerpAcc = 0.f;

	m_eTopoda = TOPODA::TOPODA_PUNCH;

	m_pTransformCom->Rotate(0.f, -30.f, 0.f);

	_vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK) * -100.f;
	m_pTransformCom->Locate(vLook._float3());

	m_pModelCom->SetUp_Animation(m_iAnimIndex[static_cast<_uint>(m_eTopoda)], false);

	_mat matSocket = XMMatrixTranslation(3.5f, 0.f, 0.5f);
	m_matSocket = matSocket._float4x4();
}

void CSkill_Topoda::Despawn()
{
	m_bActive = false;
	m_bAttach = true;

	m_fLerpAcc = 0.f;

	m_pAutoLockTarget = nullptr;

	m_pMovementCom->Reset_Velocity();
}

void CSkill_Topoda::Toggle_Collider(_bool bToggle)
{
	m_pColliderCom->Set_Active(bToggle);
}

void CSkill_Topoda::Dettach()
{
	m_bAttach = false;

	_vec3 vPos = _mat(m_CombinedWorldMatrix).position;
	_vec4 vRot = CUtils::Get_MatrixQuaternion(_mat(m_CombinedWorldMatrix)._matrix());

	m_pTransformCom->Locate(vPos._float3());
	m_pTransformCom->Rotate(vRot._vector());
}

void CSkill_Topoda::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case (_uint)COLLISION_GROUP::MONSTER :
	{
		_float fDamage = m_pOwner->Get_PlayerStat()->fDamageFactor * 20.f;
		
		IDamaged* pDamaged = dynamic_cast<IDamaged*>(pOther);
		if (pDamaged == nullptr)
			return;

		pDamaged->Damaged(fDamage, IDamaged::DAMAGE::DAMAGE_HEAVY, this);
	}
	break;
	default :
		break;
	}
}

void CSkill_Topoda::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CSkill_Topoda::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CSkill_Topoda::Ready_Component()
{
	// Model
	{
		CModel::DESC Desc{};

		Desc.pOwner = this;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Topoda",
			L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
			return E_FAIL;
	}

	// Com_Shader
	{
		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
			L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::PLAYER_SKILL;

		Desc.fRadius = 5.f;
		Desc.vCenter = { 0.f, 1.f, 5.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}

	// Com_Movement
	{
		CMovement::DESC tMovementDesc{};
		tMovementDesc.fForceFriction = 10.f;
		tMovementDesc.fGravityScale = 0.f;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
			L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &tMovementDesc)))
			return E_FAIL;

		m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
	}

	return S_OK;
}

HRESULT CSkill_Topoda::Ready_Event()
{
	// Collider 이벤트 등록
	if (FAILED(Add_Event(L"Event_MantisPunch_ColliderOn", CEvent_MantisPunch_ColliderOn::Create(this))) ||
		FAILED(Add_Event(L"Event_MantisPunch_ColliderOff", CEvent_MantisPunch_ColliderOff::Create(this))))
		return E_FAIL;

	// Dettach 이벤트 등록
	if (FAILED(Add_Event(L"Event_MantisPunch_Dettach", CEvent_MantisPunch_Dettach::Create(this))))
		return E_FAIL;

	// Retreat 이벤트 등록
	if (FAILED(Add_Event(L"Event_MantisPunch_Retreat", CEvent_MantisPunch_Retreat::Create(this))))
		return E_FAIL;

	// 맨티스 펀치 이펙트 이벤트 등록(왼손)
	const _float4x4* pLeftFistSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name({ "fist_L_end" });
	const _float4x4* pLeftHandSocketMatrix = &m_pModelCom->Get_BoneCombinedMatrix_By_Name({ "Hand_L" });
	if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Mantis_Charging",
		CEvent_Effect_MantisPunch_Topoda::Create(this, pLeftHandSocketMatrix,
			CEvent_Effect_MantisPunch_Topoda::CHARGE))))
		return E_FAIL;
	if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Mantis_TeleportStart",
		CEvent_Effect_MantisPunch_Topoda::Create(this, pLeftHandSocketMatrix,
			CEvent_Effect_MantisPunch_Topoda::TELE_START))))
		return E_FAIL;
	if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Mantis_TeleportEnd",
		CEvent_Effect_MantisPunch_Topoda::Create(this, pLeftHandSocketMatrix,
			CEvent_Effect_MantisPunch_Topoda::TELE_END))))
		return E_FAIL;
	if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Mantis_Slash",
		CEvent_Effect_MantisPunch_Topoda::Create(this, pLeftFistSocketMatrix,
			CEvent_Effect_MantisPunch_Topoda::SLASH))))
		return E_FAIL;
	if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Mantis_Impact",
		CEvent_Effect_MantisPunch_Topoda::Create(this, pLeftFistSocketMatrix,
			CEvent_Effect_MantisPunch_Topoda::IMPACT))))
		return E_FAIL;
	if (FAILED(Add_Event(L"Event_Effect_MantisPunch_Mantis_JumpOut",
		CEvent_Effect_MantisPunch_Topoda::Create(this, pLeftHandSocketMatrix,
			CEvent_Effect_MantisPunch_Topoda::JUMP_OUT))))
		return E_FAIL;


	return S_OK;
}

CSkill_Topoda* CSkill_Topoda::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Topoda* pInstance = new CSkill_Topoda(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CSkill_Topoda");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSkill_Topoda::Clone(void* pArg)
{
	CSkill_Topoda* pInstance = new CSkill_Topoda(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSkill_Topoda");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Topoda::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMovementCom);
}