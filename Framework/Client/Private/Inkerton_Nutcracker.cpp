#include "pch.h"
#include "Inkerton_Nutcracker.h"

#include "GameInstance.h"
#include "Player.h"
#include "Boss_Inkerton.h"
#include "CineCamera.h"

CInkerton_Nutcracker::CInkerton_Nutcracker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CInkerton_Nutcracker::CInkerton_Nutcracker(const CInkerton_Nutcracker& Prototype)
	: super(Prototype)
{
}

HRESULT CInkerton_Nutcracker::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInkerton_Nutcracker::Initialize(void* pArg)
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

void CInkerton_Nutcracker::Priority_Update(_float fTimeDelta)
{
}

void CInkerton_Nutcracker::Update(_float fTimeDelta)
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


	////_matrix translationMatrix = XMMatrixTranslation(offset.x, offset.y, offset.z);
	////_matrix socketMatrixWithOffset = XMLoadFloat4x4(m_pSocketMatrix) * translationMatrix;
	////XMStoreFloat4x4(&m_pGrabMatrix, socketMatrixWithOffset);

	//XMStoreFloat4x4(&m_pGrabMatrix, XMLoadFloat4x4(m_pSocketMatrix));
	//XMMatrixRotationZ(90.f) * 
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	m_pHandleColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CInkerton_Nutcracker::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pHandleColliderCom);

#endif // _DEBUG
}

HRESULT CInkerton_Nutcracker::Render()
{
	return S_OK;
}

void CInkerton_Nutcracker::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)		
	{				
		CBoss_Inkerton* pInkerton = static_cast<CBoss_Inkerton*>(m_pOwner);
		if (pInkerton->Is_Grab() && !pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible && !pPlayer->Get_PlayerStat()->bEvade && !pPlayer->Get_PlayerStat()->bBackStep) {
			pInkerton->Set_Execution(true);
			 
			m_pGrabObject = pPlayer;

			CTransform* pTransform = static_cast<CTransform*>(pInkerton->Find_Component(g_strTransformTag));

			pPlayer->Grabbed(m_pSocketMatrix, &pTransform->Get_WorldMatrix_Ref());

			CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Grab_Success.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 1.f);

			CCineCamera* pCineCamera = static_cast<CCineCamera*>(pInkerton->Find_PartObject({ L"Part_CineCamera" }));
			pCineCamera->Play(L"Inkerton_Execute", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.2f);			
			CGameInstance::GetInstance()->Camera_Transition(L"CineCamera_Inkerton", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
		}
		else if(!pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible && !pPlayer->Get_PlayerStat()->bEvade && !pPlayer->Get_PlayerStat()->bBackStep)
		{
			_vec3 vPos = m_pColliderCom->Get_BoundingCenter();
			pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
		}
	}
}

void CInkerton_Nutcracker::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CInkerton_Nutcracker::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CInkerton_Nutcracker::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

HRESULT CInkerton_Nutcracker::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;

		Desc.fRadius = 1.5f;
		Desc.vCenter = { 0.f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}

	/* Com_HandleCollider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = 1.5f;
		Desc.vCenter = { 0.f, -3.5f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_HandleCollider", reinterpret_cast<CComponent**>(&m_pHandleColliderCom), &Desc)))
			return E_FAIL;

		m_pHandleColliderCom->Set_Active(false);
	}

	return S_OK;
}

CInkerton_Nutcracker* CInkerton_Nutcracker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInkerton_Nutcracker* pInstance = new CInkerton_Nutcracker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CInkerton_Nutcracker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CInkerton_Nutcracker::Clone(void* pArg)
{
	CInkerton_Nutcracker* pInstance = new CInkerton_Nutcracker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CInkerton_Nutcracker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Nutcracker::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pHandleColliderCom);
}
