#include "pch.h"
#include "Roland_Fist.h"

#include "GameInstance.h"

#include "Player.h"

CRoland_Fist::CRoland_Fist(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CRoland_Fist::CRoland_Fist(const CRoland_Fist& Prototype)
	: super(Prototype)
{
}

HRESULT CRoland_Fist::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoland_Fist::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
	{
		MSG_BOX("there is no Socket Matrix Ptr");
		return E_FAIL;
	}
	else
	{
		m_pSocketMatrix = pDesc->pSocketMatrix;
		m_fDamage = pDesc->fDamage;
		m_pOwner = pDesc->pOwner;
	}

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Locate(0.f, 0.f, 0.f);

	return S_OK;
}

void CRoland_Fist::Priority_Update(_float fTimeDelta)
{
}

void CRoland_Fist::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CRoland_Fist::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CRoland_Fist::Render()
{
	return S_OK;
}

void CRoland_Fist::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

void CRoland_Fist::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr) {
		m_bIsHit = true;
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_SUPER, m_pOwner);
		CGameInstance::GetInstance()->Play_Sound(L"Roland_PunchHit.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);		
	}		
}

void CRoland_Fist::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CRoland_Fist::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CRoland_Fist::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = 1.5f;
		Desc.vCenter = { 0.f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}
	return S_OK;
}

CRoland_Fist* CRoland_Fist::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoland_Fist* pInstance = new CRoland_Fist(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CRoland_Fist");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CRoland_Fist::Clone(void* pArg)
{
	CRoland_Fist* pInstance = new CRoland_Fist(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CRoland_Fist");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Fist::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}