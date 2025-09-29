#include "pch.h"
#include "Hand_Player.h"

#include "GameInstance.h"
#include "Collider.h"

#include "Player.h"

#include "IDamaged.h"

CHand_Player::CHand_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CHand_Player::CHand_Player(const CHand_Player& Prototype)
	: super(Prototype)
{
}

HRESULT CHand_Player::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHand_Player::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (pDesc == nullptr)
	{
		MSG_BOX("Failed To Create : CHand_Player");
		return E_FAIL;
	}

	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)) ||
		FAILED(Ready_Components()))
		return E_FAIL;

	m_pOwner = pDesc->pOwner;

	m_pColliderCom->Set_Active(false);

	return S_OK;
}

void CHand_Player::Priority_Update(_float fTimeDelta)
{
}

void CHand_Player::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CHand_Player::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif
}

void CHand_Player::Final_Update(_float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
}

HRESULT CHand_Player::Render()
{
	return S_OK;
}

void CHand_Player::Collider_Toggle(_bool bToggle)
{
	m_pColliderCom->Set_Active(bToggle);
}

void CHand_Player::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case static_cast<_uint>(COLLISION_GROUP::MONSTER):
	{
		IDamaged* pDamaged = dynamic_cast<IDamaged*>(pOther);
		if (pDamaged != nullptr)
		{
			if (m_pOwner->Get_PlayerStat()->bGrappling)
				m_pGameInstance->Call_Lag(0.01f, 0.1f);
			else m_pGameInstance->Call_Lag(0.01f, 0.1f);

			_matrix CombinedWorldMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
			_vector vPos{}, vUp{}, vLook{}, vCamPos{}, vCamLook;
			vPos = CombinedWorldMatrix.r[(_uint)CTransform::STATE::POSITION];
			vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			vUp = CombinedWorldMatrix.r[(_uint)CTransform::STATE::UP];
			CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PTCL_HITSPARK, vPos, vLook, false, nullptr);

			vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
			vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
			vLook = XMLoadFloat4x4(m_pParentWorldMatrix).r[(_uint)CTransform::STATE::LOOK];
			CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vLook, vCamLook, false, nullptr);

			_uint iAttackPower = m_pOwner->Get_PlayerStat()->iAttackPower;
			_float fDamageFactor = m_pOwner->Get_PlayerStat()->fDamageFactor;
			_float fFinalDamage = m_fDamage * ((5.f * (_float)(iAttackPower)) / 100.f) * (fDamageFactor);

			IDamaged::DAMAGE eDamageType = m_pOwner->Get_PlayerStat()->eDamageType;

			pDamaged->Damaged(fFinalDamage, eDamageType, m_pOwner);
			m_pOwner->Heal_Umami(fFinalDamage);
		}
	}
	break;
	default:
		break;
	}
}

void CHand_Player::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CHand_Player::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CHand_Player::Ready_Components()
{
	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = static_cast<_uint>(COLLISION_GROUP::PLAYER_WEAPON);

		Desc.fRadius = 0.5f;
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component
		(
			LEVEL_STATIC,
			TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider",
			reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc
		)))
			return E_FAIL;
	}

	return S_OK;
}

CHand_Player* CHand_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHand_Player* pInstance = new CHand_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CHand_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CHand_Player::Clone(void* pArg)
{
	CHand_Player* pInstance = new CHand_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CHand_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHand_Player::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
}