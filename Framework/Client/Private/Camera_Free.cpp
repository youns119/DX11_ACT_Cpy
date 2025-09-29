#include "pch.h"
#include "Camera_Free.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: super(Prototype)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	super::DESC* pSuperDesc { nullptr };

	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr != pDesc)
	{
		pSuperDesc = &pDesc->CameraDesc;
		//m_fMouseSensitivity = pDesc->fMouseSensitivity;
		m_fMouseSensitivity = 0.1f;
	}

	if (FAILED(super::Initialize(pSuperDesc)))
		return E_FAIL;

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_Camera(L"Camera_Free", this);

	m_pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive) return;

	// 무조건 커서 보이게
	m_pUIManager->Show_Mouse();
#ifdef _DEBUG
	if (!m_bTyping &&
		!ImGuiFileDialog::Instance()->IsOpened("Save File") &&
		!ImGuiFileDialog::Instance()->IsOpened("Load File"))
	{
		if (m_pGameInstance->Key_Press(KEY::W))
			m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
		if (m_pGameInstance->Key_Press(KEY::S))
			m_pTransformCom->Go_Backward(fTimeDelta * m_fSpeed);
		if (m_pGameInstance->Key_Press(KEY::A))
			m_pTransformCom->Go_Left(fTimeDelta * m_fSpeed);
		if (m_pGameInstance->Key_Press(KEY::D))
			m_pTransformCom->Go_Right(fTimeDelta * m_fSpeed);

		if (m_pGameInstance->Key_Press(KEY::Q))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE::LOOK), fTimeDelta);
		if (m_pGameInstance->Key_Press(KEY::E))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE::LOOK), -fTimeDelta);

		if (m_pGameInstance->Key_Press(KEY::R))
			m_pTransformCom->Rotate(0.f, 0.f, 0.f);

		if (m_pGameInstance->Key_Press(KEY::LCONTROL))
			m_pTransformCom->Go_Toward(XMVectorSet(0.f, -1.f, 0.f, 0.f), fTimeDelta * m_fSpeed);
		if (m_pGameInstance->Key_Press(KEY::SPACE))
			m_pTransformCom->Go_Toward(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fSpeed);

		if (m_pGameInstance->Key_Press(KEY::LSHIFT))
			m_fSpeed = 5.f;
		else m_fSpeed = 1.f;

		if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_MB))
		{
			_int iMouseMove{ 0 };

			if (iMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensitivity);

			if (iMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensitivity);
		}

		if (m_pGameInstance->Key_Down(KEY::F1))
			m_pGameInstance->Camera_Transition(L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
	}
#endif

	super::Set_Transform_To_PipeLine();
}

void CCamera_Free::Update(_float fTimeDelta)
{
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::ReLocate(_vec3 vPos, _vec3 vRot)
{
	m_pTransformCom->Locate(vPos._float3());
	m_pTransformCom->Rotate(vRot.x, vRot.y, vRot.z);
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	super::Free();
}
