#include "pch.h"
#include "Shell_Item.h"

#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

CShell_Item::CShell_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CShell_Item::CShell_Item(const CShell_Item& Prototype)
	: super(Prototype)
{
}

HRESULT CShell_Item::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShell_Item::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	m_tShellStat = CShell_DataBase::Get().Get_Stat(pDesc->eShell);
	m_tShellData = CShell_DataBase::Get().Get_Data(pDesc->eShell);

	if (FAILED(super::Initialize(&pDesc->ItemDesc)) ||
		FAILED(Ready_Components()))
		return E_FAIL;

	if (pDesc->fShellCurrHP != 0.f)
		m_tShellStat.fShellCurrHP = pDesc->fShellCurrHP;

	_vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	vPos.y += 2.f;
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, vPos._vector());


	return S_OK;
}

void CShell_Item::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CShell_Item::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CShell_Item::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOW, this);

	super::Late_Update(fTimeDelta);
}

void CShell_Item::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CShell_Item::Render()
{
	return S_OK;
}

HRESULT CShell_Item::Ready_Components()
{
	// Com_Shader
	if (FAILED(super::Add_Component
	(
		LEVEL_STATIC,
		TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom)
	)))
		return E_FAIL;

	// Com_Model
	if (FAILED(Add_Component
	(
		LEVEL_STATIC,
		m_tShellData.strModel,
		L"Com_Model",
		reinterpret_cast<CComponent**>(&m_pModelCom)
	)))
		return E_FAIL;

	return S_OK;
}

void CShell_Item::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}