#include "pch.h"
#include "Petroch_LeftClaw.h"

#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Player.h"

CPetroch_LeftClaw::CPetroch_LeftClaw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPetroch_LeftClaw::CPetroch_LeftClaw(const CPetroch_LeftClaw& Prototype)
	: super(Prototype)
{
}

HRESULT CPetroch_LeftClaw::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPetroch_LeftClaw::Initialize(void* pArg)
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

void CPetroch_LeftClaw::Priority_Update(_float fTimeDelta)
{
}

void CPetroch_LeftClaw::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CPetroch_LeftClaw::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CPetroch_LeftClaw::Render()
{
	return S_OK;
}

void CPetroch_LeftClaw::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
	{
		if (!pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible)
			pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_HEAVY, m_pOwner);
	}		
}

void CPetroch_LeftClaw::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CPetroch_LeftClaw::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CPetroch_LeftClaw::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = 1.25f;
		Desc.vCenter = { 0.f, 1.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;
	
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}
	return S_OK;
}

void CPetroch_LeftClaw::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

CPetroch_LeftClaw* CPetroch_LeftClaw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPetroch_LeftClaw* pInstance = new CPetroch_LeftClaw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPetroch_LeftClaw");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPetroch_LeftClaw::Clone(void* pArg)
{
	CPetroch_LeftClaw* pInstance = new CPetroch_LeftClaw(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CPetroch_LeftClaw");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_LeftClaw::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}
