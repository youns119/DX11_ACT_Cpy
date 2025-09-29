#include "pch.h"
#include "Roland_Belly.h"

#include "GameInstance.h"

#include "Player.h"

CRoland_Belly::CRoland_Belly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CRoland_Belly::CRoland_Belly(const CRoland_Belly& Prototype)
	: super(Prototype)
{
}

HRESULT CRoland_Belly::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoland_Belly::Initialize(void* pArg)
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

void CRoland_Belly::Priority_Update(_float fTimeDelta)
{
}

void CRoland_Belly::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CRoland_Belly::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CRoland_Belly::Render()
{
	return S_OK;
}

void CRoland_Belly::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
	{		
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
	}
}

void CRoland_Belly::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CRoland_Belly::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CRoland_Belly::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = 2.5f;
		Desc.vCenter = { 0.f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}
	return S_OK;
}

CRoland_Belly* CRoland_Belly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoland_Belly* pInstance = new CRoland_Belly(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CRoland_Belly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CRoland_Belly::Clone(void* pArg)
{
	CRoland_Belly* pInstance = new CRoland_Belly(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CRoland_Belly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoland_Belly::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}