#include "pch.h"
#include "Gumpounder_Hammer.h"

#include "GameInstance.h"

#include "Player.h"

CGumpounder_Hammer::CGumpounder_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CGumpounder_Hammer::CGumpounder_Hammer(const CGumpounder_Hammer& Prototype)
	: super(Prototype)
{
}

HRESULT CGumpounder_Hammer::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGumpounder_Hammer::Initialize(void* pArg)
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

void CGumpounder_Hammer::Priority_Update(_float fTimeDelta)
{
}

void CGumpounder_Hammer::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CGumpounder_Hammer::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CGumpounder_Hammer::Render()
{
	return S_OK;
}

void CGumpounder_Hammer::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

void CGumpounder_Hammer::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
}

void CGumpounder_Hammer::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CGumpounder_Hammer::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CGumpounder_Hammer::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = 1.75f;
		Desc.vCenter = { 0.f, Desc.fRadius, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}
	return S_OK;
}

CGumpounder_Hammer* CGumpounder_Hammer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGumpounder_Hammer* pInstance = new CGumpounder_Hammer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CGumpounder_Hammer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CGumpounder_Hammer::Clone(void* pArg)
{
	CGumpounder_Hammer* pInstance = new CGumpounder_Hammer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CGumpounder_Hammer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGumpounder_Hammer::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}