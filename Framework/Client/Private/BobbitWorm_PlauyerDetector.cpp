#include "pch.h"
#include "BobbitWorm_PlayerDetector.h"

#include "GameInstance.h"
#include "Player.h"
#include "Enemy_BobbitWorm.h"

CBobbitWorm_PlayerDetector::CBobbitWorm_PlayerDetector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBobbitWorm_PlayerDetector::CBobbitWorm_PlayerDetector(const CBobbitWorm_PlayerDetector& Prototype)
	: super(Prototype)
{
}

HRESULT CBobbitWorm_PlayerDetector::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBobbitWorm_PlayerDetector::Initialize(void* pArg)
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

void CBobbitWorm_PlayerDetector::Priority_Update(_float fTimeDelta)
{
}

void CBobbitWorm_PlayerDetector::Update(_float fTimeDelta)
{
	///* calculate Combined World Matrix */
	//_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	//for (size_t i = 0; i < 3; ++i) // remove Scale;
	//	SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	//XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBobbitWorm_PlayerDetector::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	//m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);	

#endif // _DEBUG
}

HRESULT CBobbitWorm_PlayerDetector::Render()
{
	return S_OK;
}

void CBobbitWorm_PlayerDetector::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr && !pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible)
		static_cast<CEnemy_BobbitWorm*>(m_pOwner)->Ambush();
}

void CBobbitWorm_PlayerDetector::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CBobbitWorm_PlayerDetector::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CBobbitWorm_PlayerDetector::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;

		Desc.fRadius = 3.f;
		Desc.vCenter = { 0.f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);
	}

	return S_OK;
}

CBobbitWorm_PlayerDetector* CBobbitWorm_PlayerDetector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBobbitWorm_PlayerDetector* pInstance = new CBobbitWorm_PlayerDetector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CBobbitWorm_PlayerDetector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBobbitWorm_PlayerDetector::Clone(void* pArg)
{
	CBobbitWorm_PlayerDetector* pInstance = new CBobbitWorm_PlayerDetector(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CBobbitWorm_PlayerDetector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBobbitWorm_PlayerDetector::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);	
}
