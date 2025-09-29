#include "pch.h"
#include "Sardine_Horn.h"

#include "GameInstance.h"
#include "Player.h"

CSardine_Horn::CSardine_Horn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CSardine_Horn::CSardine_Horn(const CSardine_Horn& Prototype)
	: super(Prototype)
{
}

HRESULT CSardine_Horn::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSardine_Horn::Initialize(void* pArg)
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

void CSardine_Horn::Priority_Update(_float fTimeDelta)
{
}

void CSardine_Horn::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CSardine_Horn::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CSardine_Horn::Render()
{
	return S_OK;
}

void CSardine_Horn::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

void CSardine_Horn::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
}

void CSardine_Horn::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CSardine_Horn::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CSardine_Horn::Ready_Components()
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

CSardine_Horn* CSardine_Horn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSardine_Horn* pInstance = new CSardine_Horn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CSardine_Horn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSardine_Horn::Clone(void* pArg)
{
	CSardine_Horn* pInstance = new CSardine_Horn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CSardine_Horn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSardine_Horn::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}