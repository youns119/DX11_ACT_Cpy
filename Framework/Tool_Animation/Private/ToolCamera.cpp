#include "pch.h"
#include "ToolCamera.h"

#include "GameInstance.h"

#include "ImGui_Manager.h"

CToolCamera::CToolCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera{ _pDevice, _pContext }
{
}

CToolCamera::CToolCamera(const CToolCamera& _Prototype)
	: CCamera(_Prototype)
{
}

HRESULT CToolCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolCamera::Initialize(void* _pArg)
{
	__super::DESC* pSuperDesc{};

	DESC* pDesc = static_cast<DESC*>(_pArg);
	if (pDesc != nullptr)
	{
		pSuperDesc = &pDesc->CameraDesc;
		m_fMouseSensitivity = pDesc->fMouseSensitivity;

		m_vInitialEye = pDesc->CameraDesc.vEye;
		m_vInitialAt = pDesc->CameraDesc.vAt;
	}

	if (FAILED(__super::Initialize(pSuperDesc)))
		return E_FAIL;

	IMGUI->Set_ToolCamera(this);

	m_pGameInstance->Add_Camera(L"Camera_Tool", this);
	m_pGameInstance->Set_CurrCam(this);

	return S_OK;
}

void CToolCamera::Priority_Update(_float _fTimeDelta)
{
	if (!m_bActive)
		return;

	if (!IMGUI->Get_IsTyping() &&
		!ImGuiFileDialog::Instance()->IsOpened("Save File") &&
		!ImGuiFileDialog::Instance()->IsOpened("Load File"))
	{
		if (m_pGameInstance->Key_Press(KEY::W))
			m_pTransformCom->Go_Straight(_fTimeDelta * m_fCameraSpeed);
		if (m_pGameInstance->Key_Press(KEY::S))
			m_pTransformCom->Go_Backward(_fTimeDelta * m_fCameraSpeed);
		if (m_pGameInstance->Key_Press(KEY::A))
			m_pTransformCom->Go_Left(_fTimeDelta * m_fCameraSpeed);
		if (m_pGameInstance->Key_Press(KEY::D))
			m_pTransformCom->Go_Right(_fTimeDelta * m_fCameraSpeed);

		if (m_pGameInstance->Key_Press(KEY::Q))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE::LOOK), _fTimeDelta);
		if (m_pGameInstance->Key_Press(KEY::E))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE::LOOK), -_fTimeDelta);

		if (m_pGameInstance->Key_Down(KEY::R))
		{
			m_pTransformCom->Locate(m_vInitialEye);
			m_pTransformCom->LookAt(XMLoadFloat3(&m_vInitialAt));
		}

		if (m_pGameInstance->Key_Press(KEY::LSHIFT))
			m_fCameraSpeed = 5.f;
		else if (m_pGameInstance->Key_Up(KEY::LSHIFT))
			m_fCameraSpeed = 1.f;

		if (m_pGameInstance->Key_Down(KEY::F11))
		{
			m_bSlow = !m_bSlow;
			if (m_bSlow) m_fCameraSpeed = 0.1f;
			else m_fCameraSpeed = 1.f;
		}

		if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_MB))
		{
			_int iMouseMove{};

			if (iMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * iMouseMove * m_fMouseSensitivity);
			if (iMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE::RIGHT), _fTimeDelta * iMouseMove * m_fMouseSensitivity);
		}
	}

	__super::Set_Transform_To_PipeLine();
}

void CToolCamera::Update(_float _fTimeDelta)
{
}

void CToolCamera::Late_Update(_float _fTimeDelta)
{
}

HRESULT CToolCamera::Render()
{
	return S_OK;
}

void CToolCamera::ReLocate(_vec3 vPos, _vec3 vRot)
{
	m_pTransformCom->Locate(vPos._float3());
	m_pTransformCom->Rotate(vRot.x, vRot.y, vRot.z);
}

CToolCamera* CToolCamera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CToolCamera* pInstance = new CToolCamera(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CToolCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CToolCamera::Clone(void* _pArg)
{
	CToolCamera* pInstance = new CToolCamera(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed To Create : CToolCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolCamera::Free()
{
	__super::Free();
}