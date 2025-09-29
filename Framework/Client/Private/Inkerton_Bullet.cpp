#include "pch.h"
#include "Inkerton_Bullet.h"
#include "GameInstance.h"

#include "IDamaged.h"

CInkerton_Bullet::CInkerton_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CInkerton_Bullet::CInkerton_Bullet(const CInkerton_Bullet& Prototype)
	: super(Prototype)
{
}

HRESULT CInkerton_Bullet::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInkerton_Bullet::Initialize(void* pArg)
{
	CInkerton_Bullet::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;	

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	
	return S_OK;
}	

void CInkerton_Bullet::Priority_Update(_float fTimeDelta)
{	
	if (!m_bIsFired)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CInkerton_Bullet::Update(_float fTimeDelta)
{		
	if (!m_bIsFired)
		return;

	DecreaseLifeTime(fTimeDelta);
	Call_ImpactEffect(fTimeDelta);
	
	m_pTransformCom->Go_Backward(fTimeDelta);
	
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	__super::Update(fTimeDelta);
}

void CInkerton_Bullet::Late_Update(_float fTimeDelta)
{
	if (!m_bIsFired)
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);	
#endif // _DEBUG

	__super::Late_Update(fTimeDelta);
}

HRESULT CInkerton_Bullet::Render()
{
	return S_OK;
}

void CInkerton_Bullet::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{	
	if (!m_bIsFired)
		return;

	IDamaged* pDamaged = dynamic_cast<IDamaged*>(pOther);
	if (pDamaged != nullptr)
		pDamaged->Damaged(10.f, IDamaged::DAMAGE::DAMAGE_NORMAL, this);
}

void CInkerton_Bullet::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CInkerton_Bullet::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CInkerton_Bullet::DecreaseLifeTime(_float fTimeDelta)
{
	m_fCurLifeTime += fTimeDelta;

	if (m_fCurLifeTime >= m_fMaxLifeTime) {
		m_fImpactIntervalAcc = 0.f;
		m_fCurLifeTime = 0;
		m_bIsFired = false;
		m_pColliderCom->Set_Active(false);
		this->Set_IsActive(false);
	}
}

void CInkerton_Bullet::Fire_Bullet(_float4x4 StartMat)
{
	m_pColliderCom->Set_Active(true);
	m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMLoadFloat4x4(&StartMat).r[(_uint)CTransform::STATE::LOOK]);
	m_pTransformCom->Set_State(CTransform::STATE::UP, XMLoadFloat4x4(&StartMat).r[(_uint)CTransform::STATE::UP]);
	m_pTransformCom->Set_State(CTransform::STATE::RIGHT, XMLoadFloat4x4(&StartMat).r[(_uint)CTransform::STATE::RIGHT]);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat4x4(&StartMat).r[(_uint)CTransform::STATE::POSITION]);

	//m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE::POSITION));
	m_bIsFired = true;

	//_vector vDir{};
	//vDir = vDir + (m_pTransformCom->Get_State(CTransform::STATE::LOOK));
}

void CInkerton_Bullet::Call_ImpactEffect(_float fTimeDelta)
{
	m_fImpactIntervalAcc -= fTimeDelta;
	if (m_fImpactIntervalAcc <= 0.f)
	{
		_vector vPos{}, vLook{};
		vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		vLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK) * -1.f;
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT08_MESH_LASERIMPACT, vPos, vLook, false, nullptr);
		m_fImpactIntervalAcc = m_fImpactInterval;
	}
}

HRESULT CInkerton_Bullet::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;		
		Desc.fRadius = 1.5f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}

	return S_OK;
}

CInkerton_Bullet* CInkerton_Bullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInkerton_Bullet* pInstance = new CInkerton_Bullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CInkerton_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CInkerton_Bullet::Clone(void* pArg)
{
	CInkerton_Bullet* pInstance = new CInkerton_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CInkerton_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Bullet::Free()
{
	super::Free();
	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);		
}

