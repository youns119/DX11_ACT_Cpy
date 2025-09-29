#include "pch.h"
#include "Inkerton_StretchedNutcracker.h"

#include "GameInstance.h"
#include "Player.h"
#include "Boss_Inkerton.h"

CInkerton_StretchedNutcracker::CInkerton_StretchedNutcracker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CInkerton_StretchedNutcracker::CInkerton_StretchedNutcracker(const CInkerton_StretchedNutcracker& Prototype)
	: super(Prototype)
{
}

HRESULT CInkerton_StretchedNutcracker::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInkerton_StretchedNutcracker::Initialize(void* pArg)
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

void CInkerton_StretchedNutcracker::Priority_Update(_float fTimeDelta)
{
}

void CInkerton_StretchedNutcracker::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));	

	//_float3 offset(0.f, -1.0f, 0.25f);
	////_matrix translationMatrix = XMMatrixTranslation(offset.x, offset.y, offset.z);
	////_matrix socketMatrixWithOffset = XMLoadFloat4x4(m_pSocketMatrix) * translationMatrix;

	////XMStoreFloat4x4(&m_pGrabMatrix, XMMatrixRotationZ(90.f) * socketMatrixWithOffset);
 //	
	///*_matrix translationMatrix = XMMatrixTranslation(offset.x, offset.y, offset.z);
	//_matrix socketMatrixWithOffset = XMLoadFloat4x4(m_pSocketMatrix) * translationMatrix;
	//XMStoreFloat4x4(&m_pGrabMatrix, socketMatrixWithOffset);*/

	//XMStoreFloat4x4(&m_pGrabMatrix, XMLoadFloat4x4(m_pSocketMatrix));
	// XMMatrixRotationZ(90.f) *			
}

void CInkerton_StretchedNutcracker::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CInkerton_StretchedNutcracker::Render()
{
	return S_OK;
}

void CInkerton_StretchedNutcracker::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)		
	{
		if (!pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible)
		{
			_vec3 vPos = m_pColliderCom->Get_BoundingCenter();
			pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
		}
	}
}

void CInkerton_StretchedNutcracker::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CInkerton_StretchedNutcracker::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CInkerton_StretchedNutcracker::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

HRESULT CInkerton_StretchedNutcracker::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = 2.f;
		Desc.vCenter = { 0.f, -3.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
			
		m_pColliderCom->Set_Active(false);
	}

	return S_OK;
}

CInkerton_StretchedNutcracker* CInkerton_StretchedNutcracker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInkerton_StretchedNutcracker* pInstance = new CInkerton_StretchedNutcracker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CInkerton_StretchedNutcracker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CInkerton_StretchedNutcracker::Clone(void* pArg)
{
	CInkerton_StretchedNutcracker* pInstance = new CInkerton_StretchedNutcracker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CInkerton_StretchedNutcracker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_StretchedNutcracker::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);	
}