#include "pch.h"
#include "Pagurus_Fork.h"

#include "GameInstance.h"
#include "Player.h"
#include "Boss_Pagurus.h"

CPagurus_Fork::CPagurus_Fork(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPagurus_Fork::CPagurus_Fork(const CPagurus_Fork& Prototype)
	: super(Prototype)
{
}

HRESULT CPagurus_Fork::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPagurus_Fork::Initialize(void* pArg)
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

void CPagurus_Fork::Priority_Update(_float fTimeDelta)
{
}

void CPagurus_Fork::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
	
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));	
}

void CPagurus_Fork::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);	

#endif // _DEBUG
}

HRESULT CPagurus_Fork::Render()
{
	return S_OK;
}

void CPagurus_Fork::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)		
	{				
		CBoss_Pagurus* temp = static_cast<CBoss_Pagurus*>(m_pOwner);
		if (temp->Is_Grab() && !pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible) {
			temp->Set_Execution(true);

			m_pGrabObject = pPlayer;

			CTransform* pTransform = static_cast<CTransform*>(temp->Find_Component(g_strTransformTag));

			pPlayer->Grabbed(m_pSocketMatrix, &pTransform->Get_WorldMatrix_Ref());

			CGameInstance::GetInstance()->Play_Sound(L"Pagurus_GrabSuccessSlam.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE1, .5f);
		}
		else if(!pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible)
		{
			_vec3 vPos = m_pColliderCom->Get_BoundingCenter();
			pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
		}
	}
}

void CPagurus_Fork::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CPagurus_Fork::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CPagurus_Fork::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

HRESULT CPagurus_Fork::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;

		Desc.fRadius = 5.5f;
		Desc.vCenter = { 0.f, -5.5f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}

	return S_OK;
}

CPagurus_Fork* CPagurus_Fork::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPagurus_Fork* pInstance = new CPagurus_Fork(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPagurus_Fork");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPagurus_Fork::Clone(void* pArg)
{
	CPagurus_Fork* pInstance = new CPagurus_Fork(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CPagurus_Fork");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPagurus_Fork::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);	
}
