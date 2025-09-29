#include "pch.h"
#include "Inkerton_Gun.h"

#include "GameInstance.h"
#include "Player.h"

#include "Inkerton_Bullet.h"

CInkerton_Gun::CInkerton_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CInkerton_Gun::CInkerton_Gun(const CInkerton_Gun& Prototype)
	: super(Prototype)
{
}

HRESULT CInkerton_Gun::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInkerton_Gun::Initialize(void* pArg)
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

	if (FAILED(Ready_Bullets()))
		return E_FAIL;
	
	m_pTransformCom->Locate(0.f, 0.f, 0.f);

	return S_OK;
}

void CInkerton_Gun::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iBulletCount; i++)
		m_Bullets[i]->Priority_Update(fTimeDelta);

	super::Priority_Update(fTimeDelta);
}

void CInkerton_Gun::Update(_float fTimeDelta)
{	
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));	

	for (size_t i = 0; i < m_iBulletCount; i++)
		m_Bullets[i]->Update(fTimeDelta);

	super::Update(fTimeDelta);
}

void CInkerton_Gun::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iBulletCount; i++)
		m_Bullets[i]->Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);	

#endif // _DEBUG

	super::Late_Update(fTimeDelta);
}

HRESULT CInkerton_Gun::Render()
{
	return S_OK;
}

HRESULT CInkerton_Gun::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;

		Desc.fRadius = 2.f;
		Desc.vCenter = { 0.f, 0.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}
	
	return S_OK;
}

HRESULT CInkerton_Gun::Ready_Bullets()
{
	for (_int i = 0; i < m_iBulletCount; i++)
	{
		CInkerton_Bullet::DESC BulletDesc{};
		BulletDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 170.f;

		CInkerton_Bullet* pBullet = static_cast<CInkerton_Bullet*>(m_pGameInstance->Clone_Prototype(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Inkerton_Bullet", &BulletDesc));
		m_Bullets.push_back(pBullet);
		pBullet->Set_IsActive(false);
	}

	return S_OK;
}

void CInkerton_Gun::Fire_Bullet()
{
	m_Bullets[m_iCurBulletIndex]->Set_IsActive(true);
	m_Bullets[m_iCurBulletIndex]->Fire_Bullet(m_CombinedWorldMatrix);
	m_iCurBulletIndex++;
	m_iCurBulletIndex %= m_iBulletCount;

	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Fire_1.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 1.f);
}

CInkerton_Gun* CInkerton_Gun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInkerton_Gun* pInstance = new CInkerton_Gun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CInkerton_Gun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CInkerton_Gun::Clone(void* pArg)
{
	CInkerton_Gun* pInstance = new CInkerton_Gun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CInkerton_Gun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Gun::Free()
{
	super::Free();
		
	for (auto& pObject : m_Bullets)
		Safe_Release(pObject);

	Safe_Release(m_pColliderCom);		
}
