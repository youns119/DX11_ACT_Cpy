#include "pch.h"
#include "Cuttlefish_GrabTentacle.h"

#include "GameInstance.h"
#include "Player.h"
#include "Enemy_Cuttlefish.h"
#include "CineCamera.h"

CCuttlefish_GrabTentacle::CCuttlefish_GrabTentacle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CCuttlefish_GrabTentacle::CCuttlefish_GrabTentacle(const CCuttlefish_GrabTentacle& Prototype)
	: super(Prototype)
{
}

HRESULT CCuttlefish_GrabTentacle::Initialize_Prototype()
{	
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCuttlefish_GrabTentacle::Initialize(void* pArg)
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

void CCuttlefish_GrabTentacle::Priority_Update(_float fTimeDelta)
{
}

void CCuttlefish_GrabTentacle::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CCuttlefish_GrabTentacle::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CCuttlefish_GrabTentacle::Render()
{
	return S_OK;
}

void CCuttlefish_GrabTentacle::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

void CCuttlefish_GrabTentacle::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
	{
		CEnemy_Cuttlefish* pCuttlefish = static_cast<CEnemy_Cuttlefish*>(m_pOwner);
		if (pCuttlefish->Is_Grab() && !pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible) {
			pCuttlefish->Set_Execution(true);

			m_pGrabObject = pPlayer;

			CTransform* pTransform = static_cast<CTransform*>(pCuttlefish->Find_Component(g_strTransformTag));
			pPlayer->Grabbed(m_pSocketMatrix, &pTransform->Get_WorldMatrix_Ref());
			
			CCineCamera* pCineCamera = static_cast<CCineCamera*>(pCuttlefish->Find_PartObject({ L"Part_CineCamera" }));
			pCineCamera->Play(L"Execute", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.f);

			_uint iID = pCuttlefish->Get_ID();
			_wstring strCamName = to_wstring(iID);
			CGameInstance::GetInstance()->Camera_Transition(strCamName, (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
		}
	}
}

void CCuttlefish_GrabTentacle::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CCuttlefish_GrabTentacle::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CCuttlefish_GrabTentacle::Ready_Components()
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

CCuttlefish_GrabTentacle* CCuttlefish_GrabTentacle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCuttlefish_GrabTentacle* pInstance = new CCuttlefish_GrabTentacle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCuttlefish_GrabTentacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCuttlefish_GrabTentacle::Clone(void* pArg)
{
	CCuttlefish_GrabTentacle* pInstance = new CCuttlefish_GrabTentacle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CCuttlefish_GrabTentacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCuttlefish_GrabTentacle::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}