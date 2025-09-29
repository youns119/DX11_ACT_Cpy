#include "pch.h"
#include "Bowman_Band.h"
#include "GameInstance.h"

#include "Player.h"

CBowman_Band::CBowman_Band(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBowman_Band::CBowman_Band(const CBowman_Band& Prototype)
	: super(Prototype)
{
}

HRESULT CBowman_Band::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBowman_Band::Initialize(void* pArg)
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

void CBowman_Band::Priority_Update(_float fTimeDelta)
{
}

void CBowman_Band::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBowman_Band::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CBowman_Band::Render()
{
	return S_OK;
}

void CBowman_Band::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

void CBowman_Band::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_HEAVY, m_pOwner);
}

void CBowman_Band::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CBowman_Band::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CBowman_Band::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = 1.f;
		Desc.vCenter = { 0.5f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}
	return S_OK;
}

CBowman_Band* CBowman_Band::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBowman_Band* pInstance = new CBowman_Band(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CBowman_Band");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBowman_Band::Clone(void* pArg)
{
	CBowman_Band* pInstance = new CBowman_Band(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CBowman_Band");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowman_Band::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}