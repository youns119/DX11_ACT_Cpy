#include "pch.h"
#include "AnimObject.h"

#include "GameInstance.h"

#include "ToolPartObject.h"
#include "CineCamera_Tool.h"
#include "TestEvent1.h"
#include "TestEvent2.h"
#include "ImGui_Manager.h"

CAnimObject::CAnimObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject{ _pDevice, _pContext }
{
}

CAnimObject::CAnimObject(const CAnimObject& _Prototype)
	: CGameObject(_Prototype)
{
}

HRESULT CAnimObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObject::Initialize(void* _pArg)
{
	DESC* pDesc = static_cast<DESC*>(_pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize(&pDesc->GameObjectDesc)) ||
		FAILED(Ready_Components(pDesc->strModelTag, pDesc->eLevelID)) ||
		FAILED(Ready_PartObject()) ||
		FAILED(Ready_Events()))
		return E_FAIL;

	IMGUI->Set_AnimObject(this);
	IMGUI->Set_PartObject(static_cast<CToolPartObject*>((Find_PartObject({ L"PartObject" }))));

	return S_OK;
}

void CAnimObject::Priority_Update(_float _fTimeDelta)
{
	super::Priority_Update(_fTimeDelta);
}

void CAnimObject::Update(_float _fTimeDelta)
{
	if (m_pModelCom == nullptr)
		return;

	m_pModelCom->Play_Animation(_fTimeDelta);

	super::Update(_fTimeDelta);
}

void CAnimObject::Late_Update(_float _fTimeDelta)
{
	if (m_pModelCom == nullptr)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

	super::Late_Update(_fTimeDelta);
}

HRESULT CAnimObject::Render()
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
		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::DEFAULT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_ANIMMESH::BASE_COLOR;

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

_uint CAnimObject::Get_NumAnimation()
{
	return m_pModelCom->Get_NumAnimation();
}

vector<CAnimation*>* CAnimObject::Get_Animations()
{
	return m_pModelCom->Get_Animations();
}

CAnimation* CAnimObject::Get_CurrAnimation()
{
	return m_pModelCom->Get_CurrAnimation();
}

vector<CBone*>* CAnimObject::Get_Bones()
{
	return m_pModelCom->Get_Bones();
}

const _float4x4* CAnimObject::Get_CombinedMatrix(const _char* _pBoneName)
{
	return &m_pModelCom->Get_BoneCombinedMatrix_By_Name(_pBoneName);
}

void CAnimObject::SetUp_Animation(_uint _iAnimIndex, _bool _bLoop, _float _fLerpTime)
{
	m_pModelCom->SetUp_Animation(_iAnimIndex, _bLoop, _fLerpTime);
}

void CAnimObject::SetUp_Animation(_uint _iAnimIndex, _bool _bLoop)
{
	m_pModelCom->SetUp_Animation(_iAnimIndex, _bLoop);
}

void CAnimObject::Set_Animation_TrackPosition(_double _fTrackPosition)
{
	m_pModelCom->Set_AnimationFrame_Specific(_fTrackPosition);
}

void CAnimObject::Add_AnimNotify(_uint iAnimIndex, _double _fTrackPosition, string& _strNotifyName)
{
	m_pModelCom->Add_AnimNotify(iAnimIndex, _fTrackPosition, _strNotifyName);
}

void CAnimObject::Change_AnimNotify(_uint _iAnimIndex, string& _strNotifyName, string& _strChangeName, _double _fTrackPosition, _double _fChangePosition)
{
	m_pModelCom->Change_AnimNotify(_iAnimIndex, _strNotifyName, _strChangeName, _fTrackPosition, _fChangePosition);
}

void CAnimObject::Delete_AnimNotify(_uint _iAnimIndex, string& _strNotifyName, _double _fTrackPosition)
{
	m_pModelCom->Delete_AnimNotify(_iAnimIndex, _strNotifyName, _fTrackPosition);
}

HRESULT CAnimObject::Change_Model(const _wstring& _strModelTag, LEVELID _eLevelID)
{
	Delete_Component(L"Model", reinterpret_cast<CComponent**>(&m_pModelCom));
	Safe_Release(m_pModelCom);

	// Model
	CModel::DESC tModelDesc{};
	tModelDesc.pOwner = this;
	tModelDesc.pRootBone = Get_ModelRootName(_strModelTag);

	if (FAILED(Add_Component
	(
		_eLevelID,
		_strModelTag,
		L"Model",
		reinterpret_cast<CComponent**>(&m_pModelCom),
		&tModelDesc
	)))
		return E_FAIL;

	return S_OK;
}

void CAnimObject::Reset_Animation()
{
	m_pModelCom->Reset_Animation();
}

void CAnimObject::Reset_RootBone(_uint _iAnimIndex)
{
	m_pModelCom->Reset_RootBone(_iAnimIndex);
}

HRESULT CAnimObject::Save_Animation(_uint _iAnimIndex)
{
	if (FAILED(m_pModelCom->Save_Animation(_iAnimIndex)))
		return E_FAIL;

	return S_OK;
}

const _char* CAnimObject::Get_ModelRootName(const _wstring& _strModelName)
{
	if (_strModelName == L"Model_Player")
		return "Spine";
	else if (_strModelName == L"Model_Boss_Inkerton")
		return "Root.4";
	else return "";

	return nullptr;
}

HRESULT CAnimObject::Ready_Components(const _wstring& _strModelTag, LEVELID _eLevelID)
{
	// Shader
	if (FAILED(Add_Component
	(
		LEVEL_STATIC,
		L"Shader_VtxAnimMesh",
		L"Shader",
		reinterpret_cast<CComponent**>(&m_pShaderCom)
	)))
		return E_FAIL;

	// Model
	CModel::DESC tModelDesc{};
	tModelDesc.pOwner = this;
	tModelDesc.pRootBone = Get_ModelRootName(_strModelTag);

	if (FAILED(Add_Component
	(
		_eLevelID,
		_strModelTag,
		L"Model",
		reinterpret_cast<CComponent**>(&m_pModelCom),
		&tModelDesc
	)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObject::Ready_PartObject()
{
	// PartObject
	{
		CToolPartObject::DESC tPartObjectDesc{};

		tPartObjectDesc.PartObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();
		tPartObjectDesc.pSocketMatrix = nullptr;

		if (FAILED(Add_PartObject(L"PartObject", LEVEL_TOOL, L"GameObject_PartObject", &tPartObjectDesc)))
			return E_FAIL;
	}

	// CineCamera
	{
		CCineCamera_Tool::DESC tCineCameraDesc{};
		tCineCameraDesc.CameraDesc.GameObjectDesc.pParentWorldMatrix = &m_pTransformCom->Get_WorldMatrix_Ref();

		if (FAILED(Add_PartObject(L"CineCamera", LEVEL_TOOL, L"GameObject_CineCamera_Tool", &tCineCameraDesc)))
			return E_FAIL;

		m_pCineCamera = static_cast<CCineCamera_Tool*>(Find_PartObject({ L"CineCamera" }));
	}

	return S_OK;
}

HRESULT CAnimObject::Ready_Events()
{
	if (FAILED(Add_Event(L"Test1", CTestEvent1::Create(this))) ||
		FAILED(Add_Event(L"Test2", CTestEvent2::Create(this))))
		return E_FAIL;

	return S_OK;
}

CAnimObject* CAnimObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CAnimObject* pInstance = new CAnimObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CAnimObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CAnimObject::Clone(void* _pArg)
{
	CAnimObject* pInstance = new CAnimObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed To Create : CAnimObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimObject::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}