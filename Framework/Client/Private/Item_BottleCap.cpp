#include "pch.h"
#include "Item_BottleCap.h"

#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

#include "Player.h"
#include "UI_Manager.h"
#include "CutScene_Manager.h"

CItem_BottleCap::CItem_BottleCap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

CItem_BottleCap::CItem_BottleCap(const CItem_BottleCap& Prototype)
    : super(Prototype)
{
}

HRESULT CItem_BottleCap::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CItem_BottleCap::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(&pDesc->ItemDesc)) ||
		FAILED(Ready_Components()))
		return E_FAIL;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	m_pCutSceneManager = static_cast<CCutScene_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_CutSceneManager", 0));

	return S_OK;
}

void CItem_BottleCap::Priority_Update(_float fTimeDelta)
{
}

void CItem_BottleCap::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CItem_BottleCap::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom); 
#endif

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
}

void CItem_BottleCap::Final_Update(_float fTimeDelta)
{
}

HRESULT CItem_BottleCap::Render()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	_uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iShaderPass = 0;
		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

			if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
			return E_FAIL;

		m_pShaderCom->Begin(iShaderPass);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CItem_BottleCap::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case (_uint)COLLISION_GROUP::PLAYER:
	{
		m_pUIManager->Show_Interaction(L"ащ╠Б");

		CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
		if (pPlayer == nullptr)
			return;

		pPlayer->Clear_Buffer();
	}
	break;
	default:
		break;
	}
}

void CItem_BottleCap::On_Collision(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case (_uint)COLLISION_GROUP::PLAYER:
	{
		if (m_pGameInstance->Key_Down(KEY::E))
		{
			m_pUIManager->Hide_Interaction();

			m_pUIManager->Get_QuestItemInventory()->Add(0, 1);
			m_pCutSceneManager->Begin_CutScene(L"ShellSplitter");
			m_pGameInstance->Delete_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_MapItem", this);

			m_pGameInstance->Stop_Sound((_uint)SOUND_CHANNEL::BGM);

			CPlayer* pPlayer = static_cast<CPlayer*>(pOther);
			if (pPlayer == nullptr)
				return;

			pPlayer->Clear_Buffer();
		}
	}
	break;
	default :
		break;
	}
}

void CItem_BottleCap::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
	m_pUIManager->Hide_Interaction();
}

HRESULT CItem_BottleCap::Ready_Components()
{
	// Com_Model
	if (FAILED(Add_Component
	(
		LEVEL_STATIC,
		L"Prototype_Component_Model_Item_BottleCap",
		L"Com_Model",
		reinterpret_cast<CComponent**>(&m_pModelCom)
	)))
		return E_FAIL;

	// Com_Shader
	if (FAILED(super::Add_Component
	(
		LEVEL_STATIC,
		TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom)
	)))
		return E_FAIL;

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

CItem_BottleCap* CItem_BottleCap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_BottleCap* pInstance = new CItem_BottleCap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CItem_BottleCap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CItem_BottleCap::Clone(void* pArg)
{
	CItem_BottleCap* pInstance = new CItem_BottleCap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CItem_BottleCap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_BottleCap::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}