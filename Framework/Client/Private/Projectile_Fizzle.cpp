#include "pch.h"
#include "Projectile_Fizzle.h"

#include "GameInstance.h"
#include "Collider.h"
#include "Movement.h"

#include "Player.h"
#include "Enemy.h"
#include "EffectObject.h"

#include "IDamaged.h"

CProjectile_Fizzle::CProjectile_Fizzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CProjectile_Fizzle::CProjectile_Fizzle(const CProjectile_Fizzle& Prototype)
	: super(Prototype)
{
}

HRESULT CProjectile_Fizzle::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_Fizzle::Initialize(void* pArg)
{
	CProjectile::DESC tDesc{};

	if (FAILED(super::Initialize(&tDesc)) ||
		FAILED(Ready_Components()))
		return E_FAIL;

	Deactivate();

	return S_OK;
}

void CProjectile_Fizzle::Priority_Update(_float fTimeDelta)
{
	m_fLaunchAcc += fTimeDelta;

	if (m_fLaunchAcc >= m_fLaunchTime)
	{
		if (!m_bHasTarget)
		{
			_float fDist = m_fDistance;
			for (auto& Monster : m_listMonster)
			{
				CEnemy* pEnemy = static_cast<CEnemy*>(Monster);
				if (pEnemy == nullptr || pEnemy->Get_IsDead())
					continue;

				CTransform* pTransform = static_cast<CTransform*>(pEnemy->Find_Component(g_strTransformTag));
				_float fCurrDist = _vec3(pTransform->Get_State(CTransform::STATE::POSITION) -
					m_pTransformCom->Get_State(CTransform::STATE::POSITION)).Length();

				if (fCurrDist <= fDist)
				{
					m_pTarget = pEnemy;
					m_pTargetCollider = static_cast<CCollider*>(pEnemy->Find_Component(L"Com_Collider"));
					if (m_pTargetCollider == nullptr || !m_pTargetCollider->Is_Active())
						continue;

					m_bHasTarget = true;
					fDist = fCurrDist;
				}
			}
		}
		else
		{
			if (m_pTarget->Get_IsDead())
			{
				m_pTarget = nullptr;

				m_pTargetCollider = nullptr;
				m_bHasTarget = false;
			}
			else
			{
				_vec3 vDir = m_pTargetCollider->Get_BoundingCenter() - _vec3(m_pTransformCom->Get_State(CTransform::STATE::POSITION));
				m_pMovementCom->Add_Velocity(vDir._vector(), fTimeDelta);
			}
		}
	}

	super::Priority_Update(fTimeDelta);
}

void CProjectile_Fizzle::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CProjectile_Fizzle::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CProjectile_Fizzle::Final_Update(_float fTimeDelta)
{

	super::Final_Update(fTimeDelta);
}

HRESULT CProjectile_Fizzle::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CProjectile_Fizzle::Activate(const FProjectileStat& tProjectileStat)
{
	super::Activate(tProjectileStat);

	m_listMonster = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Monster");

	m_pTransformCom->Locate(_vec3(tProjectileStat.vSrc)._float3());
	m_pMovementCom->Set_Accel(tProjectileStat.fAccel);
	m_pMovementCom->Set_Decel(tProjectileStat.fDecel);
	m_pMovementCom->Set_ForceFriction(tProjectileStat.fForceFriction);
	m_pMovementCom->Set_MinSpeed(tProjectileStat.fMinSpeed);
	m_pMovementCom->Set_MaxSpeed(tProjectileStat.fMaxSpeed);

	m_tProjectileStat.fLifeTime += (rand() % (_int)m_tProjectileStat.fLifeTime) * 0.1f;
	m_fLaunchTime = m_tProjectileStat.fLifeTime * 0.05f;

	_float fX = (_float)(rand() % 20 - 10) / 10.f;
	_float fY = (_float)(rand() % 10 + 1) / 10.f;
	_float fZ = (_float)(rand() % 20 - 10) / 10.f;

	_float fForce = tProjectileStat.fForce * 10.f;

	_vec3 vInitialDst = _vec3(fX, fY, fZ).Normalized();
	_float fInitialForce = (_float)((_int)fForce + rand() % (_int)(fForce * 0.3f)) / 10.f;

	m_pMovementCom->Add_Force(vInitialDst._vector(), fInitialForce);

	_float fRand = CUtils::Compute_Random(0.f, 2.f);
	if(fRand < 1.f)
		m_pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_BLUE, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, &m_pTransformCom->Get_WorldMatrix_Ref());
	else
		m_pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_PURPLE, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true, &m_pTransformCom->Get_WorldMatrix_Ref());
}

void CProjectile_Fizzle::Deactivate()
{
	super::Deactivate();

	m_pTarget = nullptr;
	if (nullptr != m_pEffect)
	{
		m_pEffect->Clear_Info(0.f);
		m_pEffect = nullptr;
		CEffectObject* pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_FIZZLE_BURST, m_pTransformCom->Get_State(CTransform::STATE::POSITION), XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
		if (nullptr != pEffect)
		{
			_vector vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW).r[(_uint)CTransform::STATE::LOOK];
			static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(-vCamLook);
		}

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player", 0));
		pPlayer->Play_PlayerSound(L"Shell_Fizzle_Bubble");
	}

	m_pTargetCollider = nullptr;
	m_bHasTarget = false;
	m_fLaunchAcc = 0.f;

	m_pMovementCom->Reset_Velocity();
}

void CProjectile_Fizzle::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	if (iGroup == static_cast<_uint>(COLLISION_GROUP::MONSTER))
	{
		IDamaged* pDamaged = dynamic_cast<IDamaged*>(pOther);
		if (pDamaged == nullptr)
			return;

		_vec3 vPos = m_pColliderCom->Get_BoundingCenter();
		pDamaged->Damaged(m_tProjectileStat.fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, this);
		Deactivate();
	}
}

void CProjectile_Fizzle::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CProjectile_Fizzle::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CProjectile_Fizzle::Ready_Components()
{
	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::PLAYER_PROJECTILE;

		Desc.fRadius = 0.5f;
		Desc.vCenter = { 0.f, .5f, 0.f };

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
		Desc.fForceFriction = 20.f;
		Desc.fRotationScale = 1.f;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
			L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &Desc)))
			return E_FAIL;

		m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
		m_pMovementCom->Set_RotateToDirection(true);
		m_pMovementCom->FlyMode(true);
	}

	return S_OK;
}

CProjectile_Fizzle* CProjectile_Fizzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Fizzle* pInstance = new CProjectile_Fizzle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CProjectile_Fizzle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CProjectile_Fizzle::Clone(void* pArg)
{
	CProjectile_Fizzle* pInstance = new CProjectile_Fizzle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CProjectile_Fizzle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Fizzle::Free()
{
	super::Free();
}