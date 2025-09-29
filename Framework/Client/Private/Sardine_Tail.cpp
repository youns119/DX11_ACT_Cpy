#include "pch.h"
#include "Sardine_Tail.h"

#include "GameInstance.h"

#include "Player.h"

CSardine_Tail::CSardine_Tail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CSardine_Tail::CSardine_Tail(const CSardine_Tail& Prototype)
	: super(Prototype)
{
}

HRESULT CSardine_Tail::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSardine_Tail::Initialize(void* pArg)
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

void CSardine_Tail::Priority_Update(_float fTimeDelta)
{
}

void CSardine_Tail::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CSardine_Tail::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CSardine_Tail::Render()
{
	return S_OK;
}

void CSardine_Tail::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

void CSardine_Tail::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
}

void CSardine_Tail::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CSardine_Tail::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CSardine_Tail::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = .75f;
		Desc.vCenter = { 0.f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}
	return S_OK;
}

CSardine_Tail* CSardine_Tail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSardine_Tail* pInstance = new CSardine_Tail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CSardine_Tail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSardine_Tail::Clone(void* pArg)
{
	CSardine_Tail* pInstance = new CSardine_Tail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CSardine_Tail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSardine_Tail::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}