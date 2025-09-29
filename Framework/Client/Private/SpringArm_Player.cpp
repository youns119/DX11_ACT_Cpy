#include "pch.h"
#include "SpringArm_Player.h"

#include "GameInstance.h"
#include "Collider.h"

#include "Player.h"
#include "Camera_Player.h"
#include "UI_Lock.h"

#include "Enemy.h"

CSpringArm_Player::CSpringArm_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CSpringArm_Player::CSpringArm_Player(const CSpringArm_Player& Prototype)
	: super(Prototype)
{
}

HRESULT CSpringArm_Player::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpringArm_Player::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	m_pOwner = pDesc->pOwner;
	m_fLerpSpeed = pDesc->fLerpSpeed;
	m_fMaxPitchDegree = pDesc->fMaxPitchDegree;

	m_fDefaultArmLength = pDesc->SpringArmDesc.fArmLength;

	if (FAILED(super::Initialize(pDesc)) ||
		FAILED(Ready_Component()) ||
		FAILED(Ready_PartObject()))
		return E_FAIL;

	_float3 vRotation = m_pTransformCom->Get_Rotation();
	m_fCurrPitch = m_fTargetPitch = vRotation.x;
	m_fCurrYaw = m_fTargetYaw = vRotation.y;

	return S_OK;
}

void CSpringArm_Player::Priority_Update(_float fTimeDelta)
{
	// Part_Camera Fix 여부에 따른 입력 수행
	{
		if (m_pPlayerCamera->Get_IsFixed() && m_pPlayerCamera->IsCurrCam())
			Input_Control(fTimeDelta);
	}

	super::Priority_Update(fTimeDelta);
}

void CSpringArm_Player::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CSpringArm_Player::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);

#ifdef _DEBUG

	// DebugComponent 등록
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

	_vec3 vPos = _mat(XMLoadFloat4x4(m_pParentWorldMatrix)).position + _vec3(m_vStartOffset);
	_vec3 vDir = _vec3(m_vDirOffset);
	m_pGameInstance->Add_DebugRender_Ray(vPos, vDir.Normalized() * m_fArmLength, false, _vec4(1.f, 0.f, 1.f, 0.f));

#endif
}

void CSpringArm_Player::Final_Update(_float fTimeDelta)
{
	// Part_Camera Fix 여부에 따른 입력 수행
	{
		if (m_pPlayerCamera->Get_IsFixed())
		{
			// 락온 상태일 때의 동작
			if (m_bLockOn && m_pLockOnTarget != nullptr)
			{
				_vec4 vLockOnPosition = m_pLockOnTransform->Get_State(CTransform::STATE::POSITION);
				_vec4 vPlayerPosition = _mat(XMLoadFloat4x4(m_pParentWorldMatrix)).position;
				_vec4 vDir = vLockOnPosition - vPlayerPosition;

				if (!Check_Physics())
				{
					m_pTransformCom->LookAt(vDir._vector());

					_float3 vRotation = m_pTransformCom->Get_Rotation();
					m_fCurrPitch = m_fCurrPitch = vRotation.x;
					m_fCurrYaw = m_fTargetYaw = vRotation.y;
				}
			}
			else
			{
				m_fCurrPitch = CUtils::Lerp_Float(m_fCurrPitch, m_fTargetPitch, 1.f - expf(-m_fLerpSpeed * fTimeDelta));
				m_fCurrYaw = CUtils::Lerp_Float(m_fCurrYaw, m_fTargetYaw, 1.f - expf(-m_fLerpSpeed * fTimeDelta));

				m_pTransformCom->Rotate(m_fCurrPitch, m_fCurrYaw, 0.f);
			}
		}
	}

	m_pColliderCom->Update(XMLoadFloat4x4(m_pParentWorldMatrix));

	super::Final_Update(fTimeDelta);
}

HRESULT CSpringArm_Player::Render()
{
	return S_OK;
}

CGameObject* CSpringArm_Player::Get_AutoLockTarget(_float fAutoDist) const
{
	if (m_bLockOn)
		return m_pLockOnTarget;

	CGameObject* pAutoLock{};
	_float fDist = fAutoDist;
	for (auto LockOn : m_listLockOn)
	{
		_vec4 vLockOnPosition = static_cast<CTransform*>(LockOn->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
		_vec4 vPlayerPosition = _mat(XMLoadFloat4x4(m_pParentWorldMatrix)).position;

		_vec4 vLockOnDist = vLockOnPosition - vPlayerPosition;
		_float fCurrDist = vLockOnDist.Length();

		if (fCurrDist < fDist)
		{
			fDist = fCurrDist;
			pAutoLock = LockOn;
		}
	}

	return pAutoLock;
}

void CSpringArm_Player::Zoom(_float fArmLength)
{
	m_fArmLength = fArmLength;
}

void CSpringArm_Player::UnLock_LockOn()
{
	if (nullptr != m_pLockOnTarget)
		static_cast<CEnemy*>(m_pLockOnTarget)->Set_LockedOn(false);

	m_bLockOn = false;
	m_pLockOnTarget = nullptr;
	m_pLockOnTransform = nullptr;
}

void CSpringArm_Player::Check_LockOnTarget()
{
	if (m_listLockOn.empty())
		return;

	if (nullptr != m_pLockOnTarget)
		static_cast<CEnemy*>(m_pLockOnTarget)->Set_LockedOn(false);

	m_bLockOn = !m_bLockOn;
	m_pLockOnTarget = nullptr;
	m_pLockOnTransform = nullptr;

	if (m_bLockOn)
	{
		_float fDist = m_fLockOnDist;
		for (auto LockOn : m_listLockOn)
		{
			_vec4 vLockOnPosition = static_cast<CTransform*>(LockOn->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
			_vec4 vPlayerPosition = _mat(XMLoadFloat4x4(m_pParentWorldMatrix)).position;

			_vec4 vLockOnDist = vLockOnPosition - vPlayerPosition;
			_vec4 vCameraLook = m_pTransformCom->Get_State(CTransform::STATE::LOOK);

			_float fBetweenAngle = CUtils::GetBetweenAngle(vCameraLook._vector(), vLockOnDist._vector());

			if (fBetweenAngle > m_fLockOnAngle || fBetweenAngle < -m_fLockOnAngle)
				continue;

			_float fCurrDist = vLockOnDist.Length();
			if (fCurrDist < fDist)
			{
				fDist = fCurrDist;
				m_pLockOnTarget = LockOn;
				static_cast<CEnemy*>(m_pLockOnTarget)->Set_LockedOn(true);
			}
		}

		if (m_pLockOnTarget == nullptr)
			m_bLockOn = false;
		else
			m_pLockOnTransform = static_cast<CTransform*>(m_pLockOnTarget->Find_Component(g_strTransformTag));
	}
}

void CSpringArm_Player::Sync_Rotation()
{
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Find_Component(g_strTransformTag));
	_vector vCurrRotation = CUtils::Get_MatrixQuaternion(pTransform->Get_WorldMatrix());

	XMStoreFloat4(&m_vPrevRotation, vCurrRotation);

	_vec3 vLook = pTransform->Get_State(CTransform::STATE::LOOK);
	m_pTransformCom->LookAt(vLook._vector());

	_float3 vRotation = m_pTransformCom->Get_Rotation();
	m_fCurrPitch = m_fCurrPitch = vRotation.x;
	m_fCurrYaw = m_fTargetYaw = vRotation.y;
}

void CSpringArm_Player::Reset_Rotation(_fvector vDir)
{
	m_pTransformCom->LookAt(vDir);

	_vec3 vCurrRotation = CUtils::Get_MatrixQuaternion(m_pTransformCom->Get_WorldMatrix());

	XMStoreFloat4(&m_vPrevRotation, vCurrRotation._vector());

	_float3 vRotation = m_pTransformCom->Get_Rotation();
	m_fCurrPitch = m_fCurrPitch = vRotation.x;
	m_fCurrYaw = m_fTargetYaw = vRotation.y;
}

void CSpringArm_Player::Reset_Position(_fvector vPos)
{
	XMStoreFloat4(&m_vPrevPosition, vPos);
}

void CSpringArm_Player::Input_Control(_float fTimeDelta)
{
	if (!m_bInputToggle)
		return;

	// 락온 상태 아닐 때의 조작
	if (!m_bLockOn)
	{
		// 마우스로 SpringArm 조작
		_int iMouseMove{};
		if (iMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
			m_fTargetYaw += iMouseMove * 0.1f;
		if (iMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_fTargetPitch += iMouseMove * 0.1f;
			m_fTargetPitch = CUtils::Clamp(m_fTargetPitch, -m_fMaxPitchDegree, m_fMaxPitchDegree);
		}

		// 휠로 SpringArm 길이 조작
		_int iWheelMove{};
		if (iWheelMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_Z))
		{
			m_fArmLength -= 0.01f * iWheelMove;
			if (m_fArmLength < 0.f) m_fArmLength = 0.f;
		}
	}
}

_bool CSpringArm_Player::Check_Physics()
{
	_vec3 vPosOrigin = _mat(m_pPlayerCamera->Get_CombinedMatrix()).position;
	_vec3 vPosTarget = static_cast<CCollider*>(m_pLockOnTarget->Find_Component(L"Com_Collider"))->Get_BoundingCenter();

	_vec3 vDir = _vec3(vPosTarget - vPosOrigin);
	_float fLength = vDir.Length();
	_float fDist{};

	if (m_pGameInstance->Ray_Cast_To_Physics(vPosOrigin, vDir, fLength, fDist, 1))
	{
		if (fDist < fLength)
		{
			UnLock_LockOn();
			static_cast<CPlayer*>(m_pOwner)->Player_CheckLockOn();

			return true;
		}
	}

	return false;
}

void CSpringArm_Player::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	m_listLockOn.push_back(pOther);
}

void CSpringArm_Player::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CSpringArm_Player::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
	if (m_pLockOnTarget == pOther)
	{
		UnLock_LockOn();
		static_cast<CPlayer*>(m_pOwner)->Player_CheckLockOn();
	}

	auto iter = find(m_listLockOn.begin(), m_listLockOn.end(), pOther);
	if (iter == m_listLockOn.end())
		return;

	m_listLockOn.erase(iter);
}

HRESULT CSpringArm_Player::Ready_Component()
{
	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::PLAYER_LOCKON;

		Desc.fRadius = m_fLockOnDist;
		Desc.vCenter = { 0.f, 1.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpringArm_Player::Ready_PartObject()
{
	// Camera_Player
	{
		CCamera_Player::DESC CameraDesc{};

		CameraDesc.CameraDesc.vEye = _float3(0.f, 0.f, 0.f);
		CameraDesc.CameraDesc.vAt = _float3(0.f, 0.f, 1.f);
		CameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
		CameraDesc.CameraDesc.fNear = 0.1f;
		CameraDesc.CameraDesc.fFar = 1000.f;

		CameraDesc.vOffset = _float3(0.f, 0.f, 0.f);

		// 부모 매트릭스로 CombinedMatrix 던짐
		CameraDesc.CameraDesc.GameObjectDesc.pParentWorldMatrix = &Get_CombinedMatrix();

		if (FAILED(Add_PartObject(L"Part_Camera", LEVEL_STATIC, L"Prototype_GameObject_Camera_Player", &CameraDesc)))
			return E_FAIL;

		m_pPlayerCamera = static_cast<CCamera_Player*>(Find_PartObject({ L"Part_Camera" }));
	}

	CUI_Lock::DESC LockDesc{};
	{
		LockDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/UI_LockOn.dat";
		LockDesc.pLockOn = &m_bLockOn;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Lock", LEVEL_STATIC, L"Prototype_GameObject_UI_Lock", &LockDesc)))
		return E_FAIL;


	return S_OK;
}

CSpringArm_Player* CSpringArm_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpringArm_Player* pInstance = new CSpringArm_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CSpringArm_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CSpringArm_Player::Clone(void* pArg)
{
	CSpringArm_Player* pInstance = new CSpringArm_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CSpringArm_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpringArm_Player::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
}