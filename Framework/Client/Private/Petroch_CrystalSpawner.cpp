#include "pch.h"
#include "Petroch_CrystalSpawner.h"
#include "GameInstance.h"

#include "Petroch_SharpCrystal.h"

CPetroch_CrystalSpawner::CPetroch_CrystalSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPetroch_CrystalSpawner::CPetroch_CrystalSpawner(const CPetroch_CrystalSpawner& Prototype)
	: super(Prototype)
{
}

HRESULT CPetroch_CrystalSpawner::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPetroch_CrystalSpawner::Initialize(void* pArg)
{
	CPetroch_CrystalSpawner::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;
	else 
		m_pOwner = pDesc->pOwner;
	

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Physics()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Crystals()))
		return E_FAIL;	

	if (FAILED(Ready_Sounds()))
		return E_FAIL;
	
	
	return S_OK;
}	

void CPetroch_CrystalSpawner::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_Crystals.size(); i++)
		m_Crystals[i]->Priority_Update(fTimeDelta);

	if (!m_isActive)
		return;

	m_pMovementCom->Priority_Update(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CPetroch_CrystalSpawner::Update(_float fTimeDelta)
{			
	for (size_t i = 0; i < m_Crystals.size(); i++)
		m_Crystals[i]->Update(fTimeDelta);

	if (!m_isActive)
		return;
	
	// Movement
	{
		_vector vPrePosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);		
			
		_vec2 sour = _vec2{ m_fTargetPos.x, m_fTargetPos.z };
		_vec2 temp = _vec2{ m_fStartPos.x, m_fStartPos.z };	

		if (m_fCurSplineFactor < 1.f) {
			_vec2 vTargetPos = Compute_CardinalSpline(temp, sour, m_fCurSplineFactor, m_fCurSplineTension);			
			_vector dest = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
			dest = XMVectorSetX(dest, vTargetPos.x);
			dest = XMVectorSetZ(dest, vTargetPos.y);
			m_pTransformCom->Set_State(CTransform::STATE::POSITION, dest);
			m_fCurSplineFactor += m_fMoveSpeed * fTimeDelta;
		}
		else {
			m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMLoadFloat4(&m_vDefaultLook));
			m_pTransformCom->Go_Straight(fTimeDelta * 0.4f);
		}

		Spawn_Crystal(fTimeDelta);
		m_pMovementCom->Update(fTimeDelta);

		_vector vCurPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vec3 vDisp = vCurPosition - vPrePosition;
		m_fDisatance += XMVectorGetX(XMVector3Length(vDisp._vector()));

		if (m_pPhysicsControllerCom != nullptr)
			m_pPhysicsControllerCom->Move(vDisp, static_cast<_float>(fTimeDelta * 0.5), fTimeDelta);
	}
	
	__super::Update(fTimeDelta);
}

void CPetroch_CrystalSpawner::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_Crystals.size(); i++)
		m_Crystals[i]->Late_Update(fTimeDelta);

	if (!m_isActive)
		return;	

	if (nullptr != m_pPhysicsControllerCom) {
		m_pGameInstance->Simulate_Physics(m_pPhysicsControllerCom);

		m_pMovementCom->Late_Update(fTimeDelta);
#ifdef _DEBUG		
		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pPhysicsControllerCom);
#endif // _DEBUG
	}	
	__super::Late_Update(fTimeDelta);
}

HRESULT CPetroch_CrystalSpawner::Render()
{
	for (size_t i = 0; i < m_Crystals.size(); i++)
		m_Crystals[i]->Render();

	return S_OK;
}

void CPetroch_CrystalSpawner::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{	
}

void CPetroch_CrystalSpawner::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CPetroch_CrystalSpawner::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CPetroch_CrystalSpawner::Ready_Components()
{
	if (m_pMovementCom == nullptr)
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 30.f;
		Desc.fMoveDecel = 20.f;
		Desc.fMaxSpeed = 15.f;
		Desc.fMaxFallingSpeed = 100.f;
		Desc.fRotationScale = 1;
		Desc.fGravityScale = 100.f;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
			L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &Desc)))
			return E_FAIL;

		m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
		m_pMovementCom->Set_RotateToDirection(false);
	}

	return S_OK;
}

HRESULT CPetroch_CrystalSpawner::Ready_Physics()
{
	if (m_pPhysicsControllerCom == nullptr)
	{
		CPhysics_Controller::DESC Desc{};
		Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		Desc.pFinalMatrix = const_cast<_float4x4*>(&m_pTransformCom->Get_WorldMatrix_Ref());
		Desc.fScaleCoeff = 0.5f;
		Desc.fRadius = .5f;
		Desc.fHeight = 0.3f;
		Desc.iFilter = (_uint)PHYSICS_FILTER::CONTROLLER;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics_Controller",
			L"Com_PhysicsController", reinterpret_cast<CComponent**>(&m_pPhysicsControllerCom), &Desc)))
			return E_FAIL;

		m_pPhysicsControllerCom->Set_UserData({ true });
	}

	return S_OK;
}

HRESULT CPetroch_CrystalSpawner::Ready_Crystals()
{
	for (_int i = 0; i < m_iCrystalCount; i++)
	{
		CPetroch_SharpCrystal::DESC SharpCrystalDesc{};
		SharpCrystalDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;

		CPetroch_SharpCrystal* pSharpCrystal = static_cast<CPetroch_SharpCrystal*>(m_pGameInstance->Clone_Prototype(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), L"Prototype_GameObject_Petroch_SharpCrystal", &SharpCrystalDesc));
		m_Crystals.push_back(pSharpCrystal);
		pSharpCrystal->Set_IsActive(false);
	}

	return S_OK;
}

HRESULT CPetroch_CrystalSpawner::Ready_Sounds()
{
	m_SpawnSounds.push_back(L"Petroch_CrystalSpawn1.wav");
	m_SpawnSounds.push_back(L"Petroch_CrystalSpawn2.wav");
	m_SpawnSounds.push_back(L"Petroch_CrystalSpawn3.wav");
	m_SpawnSounds.push_back(L"Petroch_CrystalSpawn4.wav");

	return S_OK;
}

void CPetroch_CrystalSpawner::Set_SpawnOption(_float4x4* m_pSocketMatrix, _vector vTargetPos, _float fTension, _float fSpawnDistance, _float fMoveSpeed, _bool bMoveLeft, _float iSpawnCount)
{	
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(0.f, 1.0f, 0.f, 1.f));

	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	for (size_t i = 0; i < 3; ++i) 
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMLoadFloat4x4(&m_CombinedWorldMatrix).r[(_uint)CTransform::STATE::LOOK]);
	m_pTransformCom->Set_State(CTransform::STATE::UP, XMLoadFloat4x4(&m_CombinedWorldMatrix).r[(_uint)CTransform::STATE::UP]);
	m_pTransformCom->Set_State(CTransform::STATE::RIGHT, XMLoadFloat4x4(&m_CombinedWorldMatrix).r[(_uint)CTransform::STATE::RIGHT]);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat4x4(&m_CombinedWorldMatrix).r[(_uint)CTransform::STATE::POSITION]);
	
	XMStoreFloat4(&m_fStartPos, m_pTransformCom->Get_State(CTransform::STATE::POSITION));
	m_fStartPos.y = XMVectorGetY(static_cast<CTransform*>(m_pOwner->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION)) + 1.f;
	XMStoreFloat4(&m_fTargetPos, vTargetPos);

	m_fCurSplineTension = fTension;
	m_fSpawnPerDistance = fSpawnDistance;
	m_fMoveSpeed = fMoveSpeed;
	m_iMaxSpawnAmount = iSpawnCount;
	m_bIsLeft = bMoveLeft;

	m_pPhysicsControllerCom->Set_Position(XMLoadFloat4(&m_fStartPos));
	
	_vec2 sour = _vec2{ m_fTargetPos.x, m_fTargetPos.z };
	_vec2 temp = _vec2{ m_fStartPos.x, m_fStartPos.z };

	_vec2 vLookStart = Compute_CardinalSpline(temp, sour, 0.9f, m_fCurSplineTension);
	_vec2 vLookEnd = Compute_CardinalSpline(temp, sour, 1.f, m_fCurSplineTension);
	_vec2 Look = vLookEnd - vLookStart;
	_vector dest = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
	dest = XMVectorSetX(dest, Look.x);
	dest = XMVectorSetZ(dest, Look.y);

	XMStoreFloat4(&m_vDefaultLook, XMVector4Normalize(dest));

	Active();
}

void CPetroch_CrystalSpawner::Deactive()
{
	//m_pMovementCom->Set_InAir(true);
	m_isActive = false;
}

void CPetroch_CrystalSpawner::Active()
{
	m_fCurSplineFactor = 0.0f;	
	m_iCurSpawnAmount = 0;
	m_fDisatance = 0;

	m_isActive = true;
}

void CPetroch_CrystalSpawner::Play_SpawnSFX()
{	
	CGameInstance::GetInstance()->Play_Sound(L"Petroch_Atk2.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE3, 1.f);
}	

void CPetroch_CrystalSpawner::Spawn_Crystal(_float fTimeDelta)
{		
	if (m_fDisatance >= m_fSpawnPerDistance) {
		CPetroch_SharpCrystal* m_Crystal = m_Crystals.at(m_iCurSpawnIndex);
		m_Crystal->Spawn(m_pTransformCom->Get_WorldMatrix_Ref());
		m_iCurSpawnAmount++;
		m_iCurSpawnIndex++;
		m_iCurSpawnIndex %= m_iCrystalCount;
		if (m_bIsLeft) {
			m_Crystal->Set_PlaySound(true);
			Play_SpawnSFX();
		}			
		else {
			m_Crystal->Set_PlaySound(false);
		}
		m_fDisatance = 0;
	}	

	if (m_iCurSpawnAmount >= m_iMaxSpawnAmount) {
		Deactive();
		return;
	}			
}

_vec2 CPetroch_CrystalSpawner::Compute_CardinalSpline(const _vec2& p0, const _vec2& p1, _float fFactor, _float fTension) {
	_vec2 vAB = p1 - p0;	
	_vec2 vLeftTangent = _vec2(-vAB.y, vAB.x); 
		
	_float m = fTension;

	if (!m_bIsLeft)
		m *= -1.f;

	_float tangentEffect = m * fFactor * (1 - fFactor);

	_vec2 result;
	result.x = p0.x * (1 - fFactor) + p1.x * fFactor + tangentEffect * vLeftTangent.x;
	result.y = p0.y * (1 - fFactor) + p1.y * fFactor + tangentEffect * vLeftTangent.y;
	
	return result;
}

CPetroch_CrystalSpawner* CPetroch_CrystalSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPetroch_CrystalSpawner* pInstance = new CPetroch_CrystalSpawner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPetroch_CrystalSpawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPetroch_CrystalSpawner::Clone(void* pArg)
{
	CPetroch_CrystalSpawner* pInstance = new CPetroch_CrystalSpawner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CPetroch_CrystalSpawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPetroch_CrystalSpawner::Free()
{
	super::Free();

	for (auto& pObject : m_Crystals)
		Safe_Release(pObject);

	Safe_Release(m_pMovementCom);
	Safe_Release(m_pPhysicsControllerCom);
}