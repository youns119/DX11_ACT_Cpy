#include "pch.h"
#include "Item_Trash.h"

#include "GameInstance.h"
#include "FileData_Locator.h"
#include "FData.h"
#include "Effect_Sprite_Loop.h"
#include "UI_Manager.h"

#include "Player.h"

CItem_Trash::CItem_Trash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CItem_Trash::CItem_Trash(const CItem_Trash& Prototype)
	: super(Prototype)
{
}

HRESULT CItem_Trash::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Trash::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;
	
	if (FAILED(super::Initialize(&pDesc->ItemDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Effects()))
		return E_FAIL;

	m_iItemNum = (_uint)CUtils::Compute_Random(0.f, 5.f);
	
	m_strTextureTag = L"Prototype_Component_Texture_Item";

	return S_OK;
}

void CItem_Trash::Priority_Update(_float fTimeDelta)
{
}

void CItem_Trash::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CItem_Trash::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
}

HRESULT CItem_Trash::Render()
{
	return S_OK;
}

void CItem_Trash::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case (_uint)COLLISION_GROUP::PLAYER:
	{
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Show_Interaction(L"ащ╠Б");
	}
	break;
	default:
		break;
	}
}

void CItem_Trash::On_Collision(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case (_uint)COLLISION_GROUP::PLAYER:
	{
		if (m_pGameInstance->Key_Down(KEY::E))
		{
			CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

			pUIManager->Add_Item(this);
			m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_MapItem", this);

			m_pGameInstance->Play_Sound(L"Pickup_Item_World.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);

			CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
			if (pPlayer == nullptr)
				return;

			pPlayer->Clear_Buffer();
		}
	}
	break;
	default:
		break;
	}
}

void CItem_Trash::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Hide_Interaction();
}

HRESULT CItem_Trash::Ready_Components()
{
	CCollider_Sphere::DESC Desc{};
	Desc.ColliderDesc.pOwner = this;
	Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::ITEM;

	Desc.fRadius = { 1.f };
	Desc.vCenter = { 0.0f, 0.0f, 0.0f };

	/* Com_Collider */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
		L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Trash::Ready_Effects()
{
	_vector vPos{}, vLook{}, vUp{};
	vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	vUp = m_pTransformCom->Get_State(CTransform::STATE::UP);
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	m_pItemEffect[PARTICLE] = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ITEM_PTCL_RESPAWN, vPos, vLook, false, nullptr);
	m_pItemEffect[SPRITE] = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ITEM_SPRITE_RESPAWN, vPos - vUp * 0.1f, vLook, false, nullptr);

	return S_OK;
}

CItem_Trash* CItem_Trash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Trash* pInstance = new CItem_Trash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CItem_Trash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CItem_Trash::Clone(void* pArg)
{
	CItem_Trash* pInstance = new CItem_Trash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CItem_Trash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Trash::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
	for (size_t i = 0; i < TYPE_END; i++)
	{
		if(nullptr != m_pItemEffect[i])
			static_cast<CEffect_Sprite_Loop*>(m_pItemEffect[i])->Clear_Info(0.f);
	}
}
