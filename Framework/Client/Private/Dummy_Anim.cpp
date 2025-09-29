#include "pch.h"
#include "Dummy_Anim.h"

#include "GameInstance.h"

CDummy_Anim::CDummy_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CDummy_Anim::CDummy_Anim(const CDummy_Anim& Prototype)
	: super(Prototype)
{
}

HRESULT CDummy_Anim::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_Anim::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->strModelTag, pDesc->eLevelID)))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(0, true, 0.2f);

	return S_OK;
}

void CDummy_Anim::Priority_Update(_float fTimeDelta)
{
}

void CDummy_Anim::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pGameInstance->Key_Down(KEY::LSHIFT))
	{
		++m_iAnimIndex;
		m_pModelCom->SetUp_Animation(m_iAnimIndex, true, 0.2f);
	}

}

void CDummy_Anim::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);
}

HRESULT CDummy_Anim::Render()
{
	if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID")))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pModelCom->Get_NumMeshes(); ++i)
	{

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		_uint iShaderPass = 0;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR; // pass base color;
			m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor");
		}
		else if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::NORMAL))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::NON_NORMAL_MAPPING;
		}

		m_pShaderCom->Begin(iShaderPass);
		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CDummy_Anim::Ready_Components(const _wstring& strModelTag, LEVELID eLevelID)
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
		L"Com_Shader", reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(Add_Component(eLevelID, strModelTag,
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CDummy_Anim* CDummy_Anim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_Anim* pInstance = new CDummy_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CDummy_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CDummy_Anim::Clone(void* pArg)
{
	CDummy_Anim* pInstance = new CDummy_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CDummy_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_Anim::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
