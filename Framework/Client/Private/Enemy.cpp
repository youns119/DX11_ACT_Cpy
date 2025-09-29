#include "pch.h"
#include "Enemy.h"
#include "GameInstance.h"

#include "Item_Trash.h"

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy& Prototype)
	: super(Prototype)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL; 

	return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
	CEnemy::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;
	
	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	m_EnemyStats = pDesc->enemyStats;
 	m_EnemyStats.fCurHP = m_EnemyStats.fMaxHP;
	m_OriginalPos = pDesc->OriginalPos;	
	m_PatrolPoints = pDesc->PatrolPoints;
	m_szName = pDesc->szName;

	m_CenterMatrixPtr = new XMFLOAT4X4();

	return S_OK;
}

void CEnemy::Priority_Update(_float fTimeDelta)
{	
	__super::Priority_Update(fTimeDelta);
}

void CEnemy::Update(_float fTimeDelta)
{			
	Dessolve(fTimeDelta);
	Flash_Effect(fTimeDelta);
	Balance_BreakTimer(fTimeDelta);
	Balance_RecoverTimer(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CEnemy::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CEnemy::Render()
{
	return S_OK;
}

void CEnemy::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

void CEnemy::Active()
{
	m_isActive = true;
}

void CEnemy::Deactive()
{
	m_isActive = false;
}

void CEnemy::Respawn()
{		
	m_isActive = true;
	m_bIsAggroed = false;
	m_bIsDead = false;
	m_bIsDessolve = false;
	m_EnemyStats.bIsCapsized = false;
	m_EnemyStats.bIsHooked = false;
	m_EnemyStats.bIsFinished = false;
	m_EnemyStats.bIsFinisher = false;
	m_fDissolveRate = 3.5f;
	
	Reset_Balance();
	Restore_Health();
	
	m_pTransformCom->Set_State(CTransform::STATE::LOOK, XMLoadFloat4x4(&m_OriginalPos).r[(_uint)CTransform::STATE::LOOK]);
	m_pTransformCom->Set_State(CTransform::STATE::UP, XMLoadFloat4x4(&m_OriginalPos).r[(_uint)CTransform::STATE::UP]);	
	m_pTransformCom->Set_State(CTransform::STATE::RIGHT, XMLoadFloat4x4(&m_OriginalPos).r[(_uint)CTransform::STATE::RIGHT]);	
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat4x4(&m_OriginalPos).r[(_uint)CTransform::STATE::POSITION]);
}

void CEnemy::Death()
{	
	m_bIsDessolve = true;
	m_EnemyStats.bIsLockedOn = false;
}

void CEnemy::Select_Action()
{
}

_bool CEnemy::Detect_Target(_float _fFovAngle, _float _fMaxDistance, _float _MinimumDistance, CTransform* _pTargetTransform, CTransform* _pBaseTransform)
{
	_float fHalfFovCos = cosf(XMConvertToRadians(_fFovAngle * 0.5f));
	_vector vLook = _pBaseTransform->Get_State(CTransform::STATE::LOOK);
	_vector vPos = _pBaseTransform->Get_State(CTransform::STATE::POSITION);
	_vector vTargetPos = _pTargetTransform->Get_State(CTransform::STATE::POSITION);
	_vector vTargetDir = XMVector3Normalize(vTargetPos - vPos);
	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPos));

	if (fDistance < _MinimumDistance)
		return true;

	if (fDistance < _fMaxDistance)
	{
		if (fHalfFovCos < XMVectorGetX(XMVector3Dot(vLook, vTargetDir)))
			return true;
	}

	return false;
}

void CEnemy::Drop_Loot()
{	
	_uint iRandomNum = (_uint)CUtils::Compute_Random(0.f, 100.f);
	if (iRandomNum > m_iDropChance)
		return;

	CItem_Trash::DESC TrashDesc{};
	{
		XMStoreFloat4x4(&TrashDesc.ItemDesc.GameObjectDesc.WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	}
	m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_Item_Trash", LEVEL_FIELD, TEXT("Layer_MapItem"), &TrashDesc);
}

void CEnemy::Hooked()
{
	CFSM* pFSM = static_cast<CFSM*>(Find_Component(L"Com_StateMachine"));
	pFSM->Call_NextState(L"Hooked");
}

_vec3 CEnemy::Get_GrapplePos()
{
	CCollider* pCollider = static_cast<CCollider*>(Find_Component(L"Com_Collider"));
	_vec3 vPos = pCollider->Get_BoundingCenter();

	return vPos;
}

_uint CEnemy::Get_Type()
{
	CCollider* pCollider = static_cast<CCollider*>(Find_Component(L"Com_Collider"));
	_uint iType = pCollider->Get_CollisionGroup();

	return iType;
}

_float CEnemy::Get_DistanceBtwTarget(_vec3 vTargetPos)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

	vTargetPos.y = 0.f;
	vPos = XMVectorSetY(vPos, 0.f);

	_vector vDis = vTargetPos._vector() - vPos;
	_float fDis = XMVectorGetX(XMVector3Length(vDis));

	return fDis;
}

void CEnemy::Restore_Health()
{	
 	m_EnemyStats.fCurHP = m_EnemyStats.fMaxHP;
}

void CEnemy::Reset_Balance()
{
	m_EnemyStats.fCurBP = 0.f;
	m_EnemyStats.fCurBalanceBreakDuration = 0.f;
	m_fCurRecoverDelay = 0.f;
	m_EnemyStats.bIsBalanceBreakable = false;	
}

void CEnemy::Balance_RecoverTimer(_float fTimeDelta)
{
	if (m_fCurRecoverDelay > m_fMaxRecoverDelay)
	{
		m_EnemyStats.fCurBP -= m_EnemyStats.fMaxBP * 0.01f;

		if (m_EnemyStats.fCurBP < 0.f)
			m_EnemyStats.fCurBP = 0.f;
	}
	else 
		m_fCurRecoverDelay += fTimeDelta;
}

void CEnemy::Dessolve(_float fTimeDelta)
{
	if (!m_bIsDessolve)
		return;

	m_fDissolveRate -= 0.015f;

	if (m_fDissolveRate < 1.8f && m_fDissolveRate > 0.5f)
		m_fSwellAmount += 0.008f;

	if (m_fDissolveRate < 0)
		Deactive();
}

void CEnemy::Flash_Effect(_float fTimeDelta)
{
	if (!m_bIsDamaged)
		return;

	if (m_fFlashIntensity > 0.6f) {
		m_fFlashIntensity -= 0.05f;
	}
	else {
		m_fFlashIntensity = 1;
		m_bIsDamaged = false;
	}
}

void CEnemy::Balance_BreakTimer(_float fTimeDelta)
{
	if (!m_EnemyStats.bIsBalanceBreakable)
		return;

	m_EnemyStats.fCurBalanceBreakDuration += fTimeDelta;

	if (m_EnemyStats.fCurBalanceBreakDuration > m_EnemyStats.fMaxBalanceBreakDuration)
		Reset_Balance();
}

void CEnemy::Free()
{
	super::Free();

	delete m_CenterMatrixPtr;
}