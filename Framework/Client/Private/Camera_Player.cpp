#include "pch.h"
#include "Camera_Player.h"

#include "GameInstance.h"
#include "CameraComponent.h"
#include "UI_Manager.h"

#include "Player.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& Prototype)
	: super(Prototype)
	, m_matOffset{ Prototype.m_matOffset }
	, m_bCameraFix{ Prototype.m_bCameraFix }
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	m_matOffset = XMMatrixIdentity();

	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
	super::DESC pSuperDesc{};

	DESC* pDesc = static_cast<DESC*>(pArg);
	if (pDesc != nullptr)
	{
		pSuperDesc = pDesc->CameraDesc;
		m_matOffset = m_matOffset * XMMatrixTranslation(pDesc->vOffset.x, pDesc->vOffset.y, pDesc->vOffset.z);
	}

	if (FAILED(super::Initialize(&pSuperDesc)) ||
		FAILED(Ready_Component()))
		return E_FAIL;

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	m_pUIManager->Hide_Mouse();

	Camera_Fix();

	m_pGameInstance->Add_Camera(L"Camera_Player", this);
	m_pGameInstance->Set_CurrCam(this);

	return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
	if (m_bActive)
	{
		// Camera 입력 수행
		if (m_pGameInstance->Key_Up(KEY::T))
		{
			m_bCameraFix = !m_bCameraFix;

			// Fix 상태일때 마우스 고정, 커서 안보임
			if (m_bCameraFix)
			{
				m_pUIManager->Hide_Mouse();
			}
			else
			{
				m_pUIManager->Show_Mouse();
			}
		}

#ifdef _DEBUG

		if (m_pGameInstance->Key_Down(KEY::F1))
			m_pGameInstance->Camera_Transition(L"Camera_Free", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);

#endif
	}

	Camera_Fix();
	Calculate_CamTransform();

	if (m_bActive)
		Set_Transform_To_PipeLine();

	super::Priority_Update(fTimeDelta);
}

void CCamera_Player::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	m_pCameraCom->Update(fTimeDelta);

	super::Update(fTimeDelta);
}

void CCamera_Player::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	// DebugComponent 등록
	m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);

	_vec3 vPos = _mat(XMLoadFloat4x4(&m_CombinedWorldMatrix)).position;
	_vec3 vDir = _mat(XMLoadFloat4x4(&m_CombinedWorldMatrix)).look;
	m_pGameInstance->Add_DebugRender_Ray(vPos, vDir, true, _vec4(1.f, 0.f, 1.f, 0.f));

#endif

	super::Late_Update(fTimeDelta);
}

void CCamera_Player::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CCamera_Player::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Player::Camera_Fix()
{
	// Fix 활성화일 때 화면 가운데로 커서 고정
	if (m_pUIManager->Get_IsActive_Mouse())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
		pPlayer->Clear_Buffer();

		m_bCameraFix = false;
		return;
	}

	m_bCameraFix = true;

	_uint2 iViewportSize = m_pGameInstance->Get_ViewportSize();
	POINT tMousePos{ static_cast<LONG>(iViewportSize.x >> 1), static_cast<LONG>(iViewportSize.y >> 1) };

	ClientToScreen(g_hWnd, &tMousePos);
	SetCursorPos(tMousePos.x, tMousePos.y);
}

void CCamera_Player::PositionShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop)
{
	m_pCameraCom->PositionShake(fAmplitude, fFrequency, fDuration, bLoop);
}

void CCamera_Player::RotationShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop)
{
	m_pCameraCom->RotationShake(fAmplitude, fFrequency, fDuration, bLoop);
}

void CCamera_Player::Stop_Shake()
{
	m_pCameraCom->Stop_Shake();
}

const list<CGameObject*>& CCamera_Player::Get_CulledTargetList(const _wstring& LayerTag, _uint iCollision, list<CGameObject*>& listCulled, _fvector vPosOrigin)
{
	list<CGameObject*> listObject = m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), LayerTag);

	for (auto& Object : listObject)
	{
		// Collision Group 검사
		CCollider* pCollider = static_cast<CCollider*>(Object->Find_Component(L"Com_Collider"));
		if (pCollider == nullptr || pCollider->Get_CollisionGroup() != iCollision || !pCollider->Is_Active())
			continue;

		//CTransform* pTransform = static_cast<CTransform*>(Object->Find_Component(g_strTransformTag));
		//_vector vPos = pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vPos = pCollider->Get_BoundingCenter()._vector();

		// Frustum 검사
		if (!m_pGameInstance->Is_InFrustum_WorldSpace(vPos))
			continue;

		// RayCast 검사
		_vec3 vRayDir = vPos - vPosOrigin;
		_float fLength = _vec3(vPos - vPosOrigin).Length();
		_float fDist{};

		if (m_pGameInstance->Ray_Cast_To_Physics(vPosOrigin, vRayDir, fLength, fDist, 1))
		{
			if (fDist < fLength)
				continue;
		}

		listCulled.push_back(Object);
	}

	return listCulled;
}

void CCamera_Player::Calculate_CamTransform()
{
	// 부모 매트릭스와의 매트릭스 계산
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_matOffset * m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentWorldMatrix));

	// 계산한 매트릭스 파이프라인으로 던짐
	_matrix matTransform = XMLoadFloat4x4(&m_CombinedWorldMatrix);

	if (m_pCameraCom->Get_IsRotationShaking())
	{
		_float3 vRotation = m_pCameraCom->Get_ShakeRotation();
		_matrix matRotation = XMMatrixRotationRollPitchYaw(vRotation.x, vRotation.y, vRotation.z);

		matTransform = matRotation * matTransform;
	}
	if (m_pCameraCom->Get_IsPositionShaking())
	{
		_float3 vTranslation = m_pCameraCom->Get_ShakePosition();
		_matrix matTranslation = XMMatrixTranslation(vTranslation.x, vTranslation.y, vTranslation.z);

		matTransform = matTranslation * matTransform;
	}

	XMStoreFloat4x4(&m_CombinedWorldMatrix, matTransform);
}

void CCamera_Player::Set_Transform_To_PipeLine()
{
	_matrix matTransform = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pGameInstance->
		Set_Transform(CPipeLine::TRANSFORM::VIEW, matTransform);
	m_pGameInstance->
		Set_Transform(CPipeLine::TRANSFORM::PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));
}

HRESULT CCamera_Player::Ready_Component()
{
	// Com_Collider
	{
		CCollider_OBB::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::DEFAULT;

		Desc.vExtents = { 0.5f, 0.5f, 0.5f };
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}

	// Com_Camera
	{
		CCameraComponent::DESC Desc{};
		Desc.pOwner = this;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Camera",
			L"Com_Camera", reinterpret_cast<CComponent**>(&m_pCameraCom), &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCamera_Player::Clone(void* pArg)
{
	CCamera_Player* pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCameraCom);
}