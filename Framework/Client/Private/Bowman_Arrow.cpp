#include "pch.h"
#include "Bowman_Arrow.h"
#include "GameInstance.h"
#include "Collider.h"

#include "Enemy_Bowman.h"
#include "Player.h"
#include "Arrow.h"	

CBowman_Arrow::CBowman_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CBowman_Arrow::CBowman_Arrow(const CBowman_Arrow& Prototype)
	: super(Prototype)
{
}

HRESULT CBowman_Arrow::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBowman_Arrow::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (pDesc == nullptr)
	{
		MSG_BOX("Failed To Create : CBowman_Arrow");
		return E_FAIL;
	}
	else {
		m_fDamage = pDesc->fDamage;
		m_pOwner = pDesc->pOwner;
	}

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Arrows()))
		return E_FAIL;

	if (pDesc->pSocketMatrix != nullptr)
		m_pSocketMatrix = pDesc->pSocketMatrix;
	if (pDesc->szFilePath != nullptr)
		Load_From_File(pDesc->szFilePath, pDesc->pModel);	

	return S_OK;
}

void CBowman_Arrow::Priority_Update(_float fTimeDelta)
{
	m_pMovementCom->Priority_Update(fTimeDelta);

	for (size_t i = 0; i < m_iArrowCount; i++)
		m_pArrows[i]->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CBowman_Arrow::Update(_float fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	for (size_t i = 0; i < m_iArrowCount; i++)
		m_pArrows[i]->Update(fTimeDelta);

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	__super::Update(fTimeDelta);
}

void CBowman_Arrow::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

	m_pMovementCom->Late_Update(fTimeDelta);

	for (size_t i = 0; i < m_iArrowCount; i++)
		m_pArrows[i]->Late_Update(fTimeDelta);


#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif // _DEBUG	
}

void CBowman_Arrow::Final_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iArrowCount; i++)
		m_pArrows[i]->Final_Update(fTimeDelta);
}

HRESULT CBowman_Arrow::Render()
{
	for (size_t i = 0; i < m_iArrowCount; i++)
		m_pArrows[i]->Render();

	if (m_bIsFired) {
		return S_OK;
	}		

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture", 0)))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CBowman_Arrow::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
		pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
}

void CBowman_Arrow::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CBowman_Arrow::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CBowman_Arrow::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

HRESULT CBowman_Arrow::Load_From_File(const _char* szFilePath, CModel* pModel)
{
	_string strLoadFile = _string(szFilePath);
	_wstring strFilePath = CUtils::ConvertToWideChar(strLoadFile);

	_bool bResult{};
	_ulong dwByte{};
	HANDLE hFile{};
	hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed To Load PartObject Data");
		return E_FAIL;
	}

	_char szSocketBone[MAX_PATH]{};

	bResult = ReadFile(hFile, &szSocketBone, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

	_float3 vScale{};
	_float3 vRotation{};
	_float3 vTranslation{};

	bResult = ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &vTranslation, sizeof(_float3), &dwByte, nullptr);

	CloseHandle(hFile);

	if (!strcmp(szSocketBone, "NoSocket"))
	{
		_float4x4 matSocket{};
		XMStoreFloat4x4(&matSocket, XMMatrixIdentity());

		m_pSocketMatrix = &matSocket;
	}
	else
		m_pSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name(szSocketBone);

	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Rotate(vRotation.x, vRotation.y, vRotation.z);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vTranslation));

	return S_OK;
}

HRESULT CBowman_Arrow::Ready_Components()
{
	// Com_Model
	if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Model_Bowman_Arrow",
		L"Com_Model", reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// Com_Shader
	if (FAILED(super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = static_cast<_uint>(COLLISION_GROUP::MONSTER_HIT);

		Desc.fRadius = 1.f;
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}

	// Com_Movement
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 100;
		Desc.fMoveDecel = 15;
		Desc.fMaxSpeed = 30;
		Desc.fMaxFallingSpeed = 10;
		Desc.fRotationScale = 0.95f;
		Desc.fGravityScale = 10;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
			L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &Desc)))
			return E_FAIL;

		m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
		m_pMovementCom->Set_RotateToDirection(false);
	}

	return S_OK;
}

HRESULT CBowman_Arrow::Ready_Arrows()
{
	for (_int i = 0; i < m_iArrowCount; i++)
	{
		CArrow::DESC ArrowDesc{};
		ArrowDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
		
		CArrow* pArrow = static_cast<CArrow*>(m_pGameInstance->Clone_Prototype(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Bowman_Arrow", &ArrowDesc));
		m_pArrows.push_back(pArrow);
		pArrow->Set_Active(false);
	}

	return S_OK;
}

void CBowman_Arrow::Trigger_Arrow(_vector vTargetPos)
{
	m_bIsFired = true;
	static_cast<CArrow*>(m_pArrows[m_iCurArrow])->Fire_Arrow(vTargetPos, m_CombinedWorldMatrix);
	m_iCurArrow++;
	m_iCurArrow %= m_iArrowCount;
}

void CBowman_Arrow::Reload_Arrow()
{
	m_bIsFired = false;
	//m_pTransformCom->Locate(_float3(0.f, 0.f, 0.f));
}

HRESULT CBowman_Arrow::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}

CBowman_Arrow* CBowman_Arrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBowman_Arrow* pInstance = new CBowman_Arrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CBowman_Arrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CBowman_Arrow::Clone(void* pArg)
{
	CBowman_Arrow* pInstance = new CBowman_Arrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CBowman_Arrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowman_Arrow::Free()
{
	super::Free();	

	for (auto& pObject : m_pArrows)
		Safe_Release(pObject);	

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMovementCom);
	//Safe_Release(m_pPhysicsControllerCom);
}
