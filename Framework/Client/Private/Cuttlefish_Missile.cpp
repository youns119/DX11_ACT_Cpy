#include "pch.h"
#include "Cuttlefish_Missile.h"
#include "GameInstance.h"

#include "Player.h"
#include "Petroch_SharpCrystal.h"

#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"


CCuttlefish_Missile::CCuttlefish_Missile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CCuttlefish_Missile::CCuttlefish_Missile(const CCuttlefish_Missile& Prototype)
	: super(Prototype)
{
}

HRESULT CCuttlefish_Missile::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCuttlefish_Missile::Initialize(void* pArg)
{
	CCuttlefish_Missile::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;
	else 
		m_pOwner = pDesc->pOwner;
	
	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Physics()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_fTrailCallTime = 0.15f;

	return S_OK;
}	

void CCuttlefish_Missile::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pMovementCom->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CCuttlefish_Missile::Update(_float fTimeDelta)
{			
	if (!m_isActive)
		return;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	_vec2 sour = _vec2{ m_fTargetPos.x, m_fTargetPos.z };
	_vec2 temp = _vec2{ m_fStartPos.x, m_fStartPos.z };

	if (m_fCurSplineFactor < 1.f) {
		_vec3 vTargetPos = Compute_CardinalSpline(temp, sour, m_fCurSplineFactor, m_fCurSplineTension, 8.f);
		_vector dest = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		dest = XMVectorSetX(dest, vTargetPos.x);
		dest = XMVectorSetY(dest, vTargetPos.y);
		dest = XMVectorSetZ(dest, vTargetPos.z);
		_vector vDir = XMVectorSetW(XMVector3Normalize(vTargetPos._vector() - dest), 0.f);
		Call_TrailEffect(vDir);
		m_pTransformCom->Set_State(CTransform::STATE::POSITION, dest);
		m_fCurSplineFactor += m_fMoveSpeed * fTimeDelta;
	}
	else {
		m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMLoadFloat4(&m_vDefaultLook));
		Call_TrailEffect(XMLoadFloat4(&m_vDefaultLook));
		m_pTransformCom->Go_Straight(fTimeDelta * 0.4f);
	}

	m_pMovementCom->Update(fTimeDelta);

	_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	if (m_fCurSplineFactor > 0.5f) {
		_vec3 vOrigin = _vec3(XMVectorGetX(vCurPosition), XMVectorGetY(vCurPosition), XMVectorGetZ(vCurPosition));
		_vec3 vDirection = _vec3(0.f, -1.f, 0.f);
		_float fMaxDistance = 0.1f;
		_float fDistance;
		_bool bHit = CGameInstance::GetInstance()->Ray_Cast_To_Physics(vOrigin, vDirection, fMaxDistance, fDistance, PxQueryFlag::eSTATIC);
		if (bHit) {
			Deactive();
		}

#ifdef _DEBUG
		ImGui::Begin("Ray Debugger");
		ImGui::NewLine();
		ImGui::Text("Collision Success: %s", bHit ? "true" : "false");
		ImGui::Text("Length: %f", fDistance);
		ImGui::End();
#endif // _DEBUG
	}
	
	__super::Update(fTimeDelta);
}

void CCuttlefish_Missile::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;	

#ifdef _DEBUG			
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif // _DEBUG

	__super::Late_Update(fTimeDelta);
}

HRESULT CCuttlefish_Missile::Render()
{
	return S_OK;
}

void CCuttlefish_Missile::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{	
	if (!m_isActive)
		return;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr) {
		pPlayer->Damaged(15.f, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
		Deactive();
	}
}

void CCuttlefish_Missile::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CCuttlefish_Missile::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CCuttlefish_Missile::Ready_Components()
{
	if (m_pMovementCom == nullptr)
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 30.f;
		Desc.fMoveDecel = 20.f;
		Desc.fMaxSpeed = 15.f;
		Desc.fMaxFallingSpeed = 1.f;
		Desc.fRotationScale = 1;
		Desc.fGravityScale = 1.f;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
			L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &Desc)))
			return E_FAIL;

		m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
		m_pMovementCom->Set_RotateToDirection(false);
	}

	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.fRadius = .4f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);
	}

	return S_OK;
}

HRESULT CCuttlefish_Missile::Ready_Physics()
{
	return S_OK;
}

void CCuttlefish_Missile::Set_SpawnOption(_float4x4* m_pSocketMatrix, _vector vTargetPos, _float fTension, _float fMoveSpeed, _bool bMoveLeft)
{	
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	for (size_t i = 0; i < 3; ++i) 
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMLoadFloat4x4(&m_CombinedWorldMatrix).r[(_uint)CTransform::STATE::LOOK]);
	m_pTransformCom->Set_State(CTransform::STATE::UP, XMLoadFloat4x4(&m_CombinedWorldMatrix).r[(_uint)CTransform::STATE::UP]);
	m_pTransformCom->Set_State(CTransform::STATE::RIGHT, XMLoadFloat4x4(&m_CombinedWorldMatrix).r[(_uint)CTransform::STATE::RIGHT]);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat4x4(&m_CombinedWorldMatrix).r[(_uint)CTransform::STATE::POSITION]);
	
	XMStoreFloat4(&m_fStartPos, m_pTransformCom->Get_State(CTransform::STATE::POSITION));	
	XMStoreFloat4(&m_fTargetPos, vTargetPos);

	m_fCurSplineTension = fTension;
	m_fMoveSpeed = fMoveSpeed;
	m_bIsLeft = bMoveLeft;	
	
	_vec2 sour = _vec2{ m_fTargetPos.x, m_fTargetPos.z };
	_vec2 temp = _vec2{ m_fStartPos.x, m_fStartPos.z };

	_vec3 vLookStart = Compute_CardinalSpline(temp, sour, 0.9f, m_fCurSplineTension, 8.f);
	_vec3 vLookEnd = Compute_CardinalSpline(temp, sour, 1.f, m_fCurSplineTension, 8.f);
	_vec3 Look = vLookEnd - vLookStart;
	_vector dest = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	dest = XMVectorSetX(dest, Look.x);
	dest = XMVectorSetY(dest, Look.y);
	dest = XMVectorSetZ(dest, Look.z);

	XMStoreFloat4(&m_vDefaultLook, XMVector4Normalize(dest));

	Active();
}

void CCuttlefish_Missile::Deactive()
{	
	CGameInstance::GetInstance()->Play_Sound(L"cuttle_cast_impact.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.5f);
	m_isActive = false;
	Disable_Effects();
	Call_ExplodeEffect();
}

void CCuttlefish_Missile::Active()
{
	m_fCurSplineFactor = 0.0f;		

	CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Cast_Splash1.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.5f);
	m_isActive = true;
	Enable_Effects();
}

void CCuttlefish_Missile::Enable_Effects()
{
	// 루프 이펙트 호출 후 저장
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);
	_vector vOffset = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	const _float4x4* pWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();

	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_MESHINST_PROJ_SPHERE, vOffset, vLook, true, pWorldMatrix, 0.f, false);
	m_Effects.push_back(m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_CIRCLE, vOffset, vLook, true, pWorldMatrix, 0.f, false));
}

void CCuttlefish_Missile::Disable_Effects()
{
	if (m_Effects.empty() == true)
		return;

	for(auto Effect : m_Effects)
	{
		if (Effect != nullptr)
		{
			Effect->Clear_Info(0.f);
			Effect = nullptr;
		}
	}
	m_Effects.clear();
}

void CCuttlefish_Missile::Call_TrailEffect(_fvector vPivot)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	CEffectObject* pEffect = nullptr;
	pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_TRAIL, vPos, vLook, false, nullptr);
	if (pEffect != nullptr)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vPivot);
}

void CCuttlefish_Missile::Call_ExplodeEffect()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_EXPLODE, vPos, vLook, false, nullptr);
}


_vec3 CCuttlefish_Missile::Compute_CardinalSpline(const _vec2& p0, const _vec2& p1, _float fFactor, _float fTension, _float fAmplitude) {
	_vec2 vAB = p1 - p0;
	_vec2 vLeftTangent = _vec2(-vAB.y, vAB.x);

	_float m = fTension;

	if (!m_bIsLeft)
		m *= -1.f;

	_float tangentEffect = m * fFactor * (1 - fFactor);

	_vec2 flatResult;
	flatResult.x = p0.x * (1 - fFactor) + p1.x * fFactor + tangentEffect * vLeftTangent.x;
	flatResult.y = p0.y * (1 - fFactor) + p1.y * fFactor + tangentEffect * vLeftTangent.y;

	_float height = sin(fFactor * 3.141592) * fAmplitude;
	
	_vec3 result3D;
	result3D.x = flatResult.x;
	result3D.y = height;        
	result3D.z = flatResult.y;  

	return result3D;
}

CCuttlefish_Missile* CCuttlefish_Missile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCuttlefish_Missile* pInstance = new CCuttlefish_Missile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCuttlefish_Missile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCuttlefish_Missile::Clone(void* pArg)
{
	CCuttlefish_Missile* pInstance = new CCuttlefish_Missile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CCuttlefish_Missile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_Missile::Free()
{
	super::Free();	

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMovementCom);	
}