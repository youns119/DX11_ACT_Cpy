#include "pch.h"
#include "Petroch_Blockable.h"
#include "GameInstance.h"
#include "Collider.h"

#include "Boss_Petroch.h"
#include "Transform.h"
#include "Player.h"

CPetroch_Blockable::CPetroch_Blockable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPetroch_Blockable::CPetroch_Blockable(const CPetroch_Blockable& Prototype)
	: super(Prototype)
{
}

HRESULT CPetroch_Blockable::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPetroch_Blockable::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (pDesc == nullptr)
	{
		MSG_BOX("Failed To Create : CPetroch_Blockable");
		return E_FAIL;
	}
	else {		
		m_pOwner = pDesc->pOwner;
	}

	if (FAILED(super::Initialize(&pDesc->PartObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pDesc->pSocketMatrix != nullptr)
		m_pSocketMatrix = pDesc->pSocketMatrix;
	if (pDesc->szFilePath != nullptr)
		Load_From_File(pDesc->szFilePath, pDesc->pModel);

	return S_OK;
}

void CPetroch_Blockable::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPetroch_Blockable::Update(_float fTimeDelta)
{	
	Reposition_Shell();

	__super::Update(fTimeDelta);
}

void CPetroch_Blockable::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
#endif // _DEBUG	

	__super::Late_Update(fTimeDelta);
}

HRESULT CPetroch_Blockable::Render()
{
	return S_OK;
}

void CPetroch_Blockable::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
	{
		if (!pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible)			
			pPlayer->KnockBack(2.f, IDamaged::DAMAGE::DAMAGE_NORMAL, this);
	}

	if (iGroup == (_uint)COLLISION_GROUP::PLAYER_PROJECTILE || iGroup == (_uint)COLLISION_GROUP::PLAYER_SKILL || iGroup == (_uint)COLLISION_GROUP::PLAYER_WEAPON || iGroup == (_uint)COLLISION_GROUP::SWING_HIT)
	{
		CBoss_Petroch* pPetroch = static_cast<CBoss_Petroch*>(m_pOwner);
		CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();

		CTransform* pPetrochTransform = static_cast<CTransform*>(pPetroch->Find_Component(TEXT("Com_Transform")));
		CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

		_vector vCurLook = pPetrochTransform->Get_State(CTransform::STATE::LOOK);
		vCurLook = XMVectorSetY(vCurLook, 0.f);
		vCurLook = XMVector3Normalize(vCurLook);

		_vector vRootLook = pPlayerTransform->Get_State(CTransform::STATE::LOOK);
		vRootLook = XMVectorSetY(vRootLook, 0.f);
		vRootLook = XMVector3Normalize(vRootLook);

		_float fDotValue = XMVectorGetX(XMVector3Dot(vCurLook, vRootLook));
		fDotValue = max(-1.0f, min(fDotValue, 1.0f));

		_float fAngle = acosf(fDotValue);
		_float fAngleDegree = XMConvertToDegrees(fAngle);

		_vector vCross = XMVector3Cross(vRootLook, vCurLook);
		_float fSign = XMVectorGetY(vCross);
		if (fSign < 0.f)
			fAngleDegree *= -1.f;

		if (abs(fAngleDegree) < 135.f || pPetroch->Is_Block()) {
			static_cast<CBoss_Petroch*>(m_pOwner)->Reset_BlockTimer();
			Call_BlockEffect(pOther);
			
			CGameInstance::GetInstance()->Play_Sound(L"Block1 - 2.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 1.f);
		}
	}
}

void CPetroch_Blockable::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CPetroch_Blockable::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CPetroch_Blockable::Reposition_Shell()
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CPetroch_Blockable::Call_BlockEffect(CGameObject* pOther)
{
	_vector vTargetPos{}, vPos{}, vLook{}, vZeroPos{};
	vTargetPos = static_cast<CCollider*>(pOther->Find_Component(L"Com_Collider"))->Get_BoundingCenter()._vector();
	vPos = _mat(m_CombinedWorldMatrix).position._vector();
	vLook = XMVectorSetW(XMVector3Normalize(vTargetPos - vPos), 0.f);

	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PETROCH_MESH_DEFEND, vLook * 2.5f, vLook, true, &m_CombinedWorldMatrix);
}

HRESULT CPetroch_Blockable::Load_From_File(const _char* szFilePath, CModel* pModel)
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

HRESULT CPetroch_Blockable::Ready_Components()
{
	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = static_cast<_uint>(COLLISION_GROUP::MONSTER);

		Desc.fRadius = 1.8f;
		Desc.vCenter = { 0.f, Desc.fRadius * 0.5f , 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(true);
	}

	return S_OK;
}


HRESULT CPetroch_Blockable::Bind_ShaderResources()
{
	return S_OK;
}

CPetroch_Blockable* CPetroch_Blockable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPetroch_Blockable* pInstance = new CPetroch_Blockable(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPetroch_Blockable");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPetroch_Blockable::Clone(void* pArg)
{
	CPetroch_Blockable* pInstance = new CPetroch_Blockable(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CPetroch_Blockable");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_Blockable::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
}
