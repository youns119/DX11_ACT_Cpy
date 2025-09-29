#include "pch.h"
#include "Splitter_LeftClaw.h"

#include "GameInstance.h"
#include "Enemy_Splitter.h"
#include "Player.h"

CSplitter_LeftClaw::CSplitter_LeftClaw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CSplitter_LeftClaw::CSplitter_LeftClaw(const CSplitter_LeftClaw& Prototype)
	: super(Prototype)
{
}

HRESULT CSplitter_LeftClaw::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSplitter_LeftClaw::Initialize(void* pArg)
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

void CSplitter_LeftClaw::Priority_Update(_float fTimeDelta)
{
}

void CSplitter_LeftClaw::Update(_float fTimeDelta)
{
	/* calculate Combined World Matrix */
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; ++i) // remove Scale;
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
	
	//_vector vOffset(XMVectorSet(0.5f, -0.5f, .0f, 1.f));
	//XMStoreFloat4x4(&m_pGrabMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_pGrabMatrix), XMMatrixRotationZ(90.f)));
	//_float3 offset(.5f, -0.5f, 0.f);
	//_matrix translationMatrix = XMMatrixTranslation(offset.x, offset.y, offset.z);

	//SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);	

	//_matrix rotationMatrix = SocketMatrix;
	//rotationMatrix.r[3] = XMVectorZero();
	//_vector quaternion = XMQuaternionRotationMatrix(rotationMatrix);


	//ImGui::Begin("Grab Debugger");

	//ImGui::NewLine();

	////_vec3 vRotation = pTransform->Get_Rotation();

	////_float m_vRotation[3] = { vRotation.x, vRotation.y, vRotation.z };
	//_float fTranslation[3] = { m_vTranslation.x, m_vTranslation.y, m_vTranslation.z };
	//_float vRotation[3] = { m_vRotation.x, m_vRotation.y, m_vRotation.z };
	//if (ImGui::DragFloat3("Rotation", vRotation, 0.01f, -180.f, 180.f))
	//	m_vRotation = _float3(vRotation[0], vRotation[1], vRotation[2]);	
	//if (ImGui::DragFloat3("Translation", fTranslation, 0.01f))
	//	m_vTranslation = _float3(fTranslation[0], fTranslation[1], fTranslation[2]);

	//ImGui::End();


	//_matrix zRotationMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMVectorGetX(quaternion) * m_vRotation.x, XMVectorGetY(quaternion) * m_vRotation.y, XMVectorGetZ(quaternion) * m_vRotation.z));
	//_matrix zRotationMatrix = XMMatrixRotationX(XMConvertToRadians(m_vRotation.x)) * XMMatrixRotationY(XMConvertToRadians(m_vRotation.y)) * XMMatrixRotationZ(XMConvertToRadians(m_vRotation.z));
	//_matrix finalRotationMatrix = XMMatrixMultiply(rotationMatrix, zRotationMatrix);
	//finalRotationMatrix.r[3] = XMVectorSet(XMVectorGetX(SocketMatrix.r[3]) * m_vTranslation.x, XMVectorGetY(SocketMatrix.r[3]) * m_vTranslation.y, XMVectorGetZ(SocketMatrix.r[3]) * m_vTranslation.z, 1.f);
	//finalRotationMatrix.r[3] = SocketMatrix.r[3];
	//_matrix rotationMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f));
	//_matrix rotationMatrix = XMMatrixRotationZ(90.f);
	//_matrix socketMatrixWithOffset = SocketMatrix * rotationMatrix *  translationMatrix;
	//_matrix finalMatrix = finalRotationMatrix;

	//XMStoreFloat4x4(&m_pGrabMatrix, finalMatrix);


	/*_matrix translationMatrix = XMMatrixTranslation(offset.x, offset.y, offset.z);
	_matrix socketMat rixWithOffset = XMLoadFloat4x4(m_pSocketMatrix) * translationMatrix;
	XMStoreFloat4x4(&m_pGrabMatrix, socketMatrixWithOffset);*/

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CSplitter_LeftClaw::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG

	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

#endif // _DEBUG
}

HRESULT CSplitter_LeftClaw::Render()
{
	return S_OK;
}

void CSplitter_LeftClaw::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
	if (pPlayer != nullptr)
	{
		CEnemy_Splitter* temp = static_cast<CEnemy_Splitter*>(m_pOwner);
		if (temp->Is_Grab() && !pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible) {
			temp->Set_Execution(true);

			m_pGrabObject = pPlayer;

			CTransform* pTransform = static_cast<CTransform*>(temp->Find_Component(g_strTransformTag));

			pPlayer->Grabbed(m_pSocketMatrix, &pTransform->Get_WorldMatrix_Ref());

			//m_pGameInstance->Camera_Transition(L"CineCamera_Splitter", 0.1f);			
		}
		else if(!pPlayer->Get_PlayerStat()->bGrabbed && !pPlayer->Get_PlayerStat()->bInvincible)
			pPlayer->Damaged(m_fDamage, IDamaged::DAMAGE::DAMAGE_NORMAL, m_pOwner);
	}		
}

void CSplitter_LeftClaw::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CSplitter_LeftClaw::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CSplitter_LeftClaw::Ready_Components()
{
	/* Com_Collider */
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		
		Desc.fRadius = 1.25f;
		Desc.vCenter = { 0.f, 1.f, 0.f };
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::MONSTER_HIT;
	
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_Active(false);
	}
	return S_OK;
}

void CSplitter_LeftClaw::Parried()
{
	dynamic_cast<IParried*>(m_pOwner)->Parried();
}

CSplitter_LeftClaw* CSplitter_LeftClaw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSplitter_LeftClaw* pInstance = new CSplitter_LeftClaw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CSplitter_LeftClaw");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSplitter_LeftClaw::Clone(void* pArg)
{
	CSplitter_LeftClaw* pInstance = new CSplitter_LeftClaw(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CSplitter_LeftClaw");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_LeftClaw::Free()
{
	super::Free();
		
	Safe_Release(m_pColliderCom);
}
