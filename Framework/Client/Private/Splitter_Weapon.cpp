#include "pch.h"
#include "Splitter_Weapon.h"

#include "GameInstance.h"
#include "Player.h"

CSplitter_Weapon::CSplitter_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CSplitter_Weapon::CSplitter_Weapon(const CSplitter_Weapon& Prototype)
	: super(Prototype)
{
}

HRESULT CSplitter_Weapon::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSplitter_Weapon::Initialize(void* pArg)
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
		m_pOwner = pDesc->pOwner;
	}

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Locate(0.f, 0.f, 0.f);
	return S_OK;
}

void CSplitter_Weapon::Priority_Update(_float fTimeDelta)
{
}

void CSplitter_Weapon::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pUpperColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	m_pLowerColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CSplitter_Weapon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pUpperColliderCom);
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pLowerColliderCom);

#endif // _DEBUG
}

HRESULT CSplitter_Weapon::Render()
{	
	return S_OK;
}

void CSplitter_Weapon::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
}

void CSplitter_Weapon::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CSplitter_Weapon::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CSplitter_Weapon::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;

		Desc.fRadius = 1.25f;
		Desc.vCenter = { 0.f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_LowerCollider", reinterpret_cast<CComponent**>(&m_pLowerColliderCom), &Desc)))
			return E_FAIL;

		m_pLowerColliderCom->Set_Active(false);
	}
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;

		Desc.fRadius = 1.25f;
		Desc.vCenter = { 0.f, 3.5f, 0.f };		
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_UpperCollider", reinterpret_cast<CComponent**>(&m_pUpperColliderCom), &Desc)))
			return E_FAIL;

		m_pUpperColliderCom->Set_Active(false);
	}
	return S_OK;
}

void CSplitter_Weapon::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

CSplitter_Weapon* CSplitter_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSplitter_Weapon* pInstance = new CSplitter_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CSplitter_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSplitter_Weapon::Clone(void* pArg)
{
	CSplitter_Weapon* pInstance = new CSplitter_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CSplitter_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Weapon::Free()
{
	super::Free();
		
	Safe_Release(m_pUpperColliderCom);
	Safe_Release(m_pLowerColliderCom);
}
