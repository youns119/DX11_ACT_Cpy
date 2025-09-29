#include "pch.h"
#include "BobbitWorm_Mouth.h"

#include "GameInstance.h"
#include "Player.h"
#include "Enemy_BobbitWorm.h"
#include "CineCamera.h"

CBobbitWorm_Mouth::CBobbitWorm_Mouth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBobbitWorm_Mouth::CBobbitWorm_Mouth(const CBobbitWorm_Mouth& Prototype)
	: super(Prototype)
{
}

HRESULT CBobbitWorm_Mouth::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBobbitWorm_Mouth::Initialize(void* pArg)
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

void CBobbitWorm_Mouth::Priority_Update(_float fTimeDelta)
{
}

void CBobbitWorm_Mouth::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));	
}

void CBobbitWorm_Mouth::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG


#endif // _DEBUG
}

HRESULT CBobbitWorm_Mouth::Render()
{
	return S_OK;
}

void CBobbitWorm_Mouth::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
	{
		CEnemy_BobbitWorm* pBobbitWorm = static_cast<CEnemy_BobbitWorm*>(m_pOwner);
		if (pBobbitWorm->Is_Grab() && !pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible) {
			pBobbitWorm->Set_Execution(true);

			m_pGrabObject = pPlayer;

			CTransform* pTransform = static_cast<CTransform*>(pBobbitWorm->Find_Component(g_strTransformTag));
			pPlayer->Grabbed(m_pSocketMatrix, &pTransform->Get_WorldMatrix_Ref());
			
			
			CGameInstance::GetInstance()->Play_Sound(L"Bobbit_Sching.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE1, 0.6f);

			CCineCamera* pCineCamera = static_cast<CCineCamera*>(pBobbitWorm->Find_PartObject({ L"Part_CineCamera" }));
			pCineCamera->Play(L"Execute", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.2f);
		/*	CGameInstance::GetInstance()->Camera_Transition(L"CineCamera_Bobbitworm", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.01f);*/
			


			_uint iID = pBobbitWorm->Get_ID();
			_wstring strCamName = to_wstring(iID);
			CGameInstance::GetInstance()->Camera_Transition(strCamName, (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
			pCineCamera->PositionShake(0.08f, 0.5f, 1.f, true);
		}
	}
}

void CBobbitWorm_Mouth::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CBobbitWorm_Mouth::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CBobbitWorm_Mouth::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;

		Desc.fRadius = 2.f;
		Desc.vCenter = { 0.f, 2.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}

	return S_OK;
}

CBobbitWorm_Mouth* CBobbitWorm_Mouth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBobbitWorm_Mouth* pInstance = new CBobbitWorm_Mouth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CBobbitWorm_Mouth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBobbitWorm_Mouth::Clone(void* pArg)
{
	CBobbitWorm_Mouth* pInstance = new CBobbitWorm_Mouth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CBobbitWorm_Mouth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBobbitWorm_Mouth::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);	
}
