#include "pch.h"
#include "CinematicCamera.h"

#include "GameInstance.h"
#include "CineCamera.h"

#include "Event_Cinematic_Shake.h"

CCinematicCamera::CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCinematicCamera::CCinematicCamera(const CCinematicCamera& Prototype)
	: super(Prototype)
{
}

HRESULT CCinematicCamera::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCinematicCamera::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)) ||
		FAILED(Ready_PartObject()) ||
		FAILED(Ready_Event()))
		return E_FAIL;

	return S_OK;
}

void CCinematicCamera::Priority_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->Key_Down(KEY::O))
	//{
	//	if (!m_pCineCamera->Get_Playing())
	//	{
	//		//m_pCineCamera->Play(L"Client_Test", L"Camera_Player",(_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
	//		m_pCineCamera->Play(L"Client_Test2", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
	//		m_pGameInstance->Camera_Transition(L"Cinematic_Camera", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
	//	}
	//	else m_pCineCamera->Stop();
	//}

	//if (m_pGameInstance->Key_Down(KEY::P))
	//{
	//	if (m_pCineCamera->Get_Playing())
	//		m_pCineCamera->Pause();
	//	else m_pCineCamera->Resume();
	//}

	super::Priority_Update(fTimeDelta);
}

void CCinematicCamera::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CCinematicCamera::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CCinematicCamera::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CCinematicCamera::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCinematicCamera::Play_Cinematic(const _wstring& strCinematic, const _wstring& strReturnName, _uint iLerp, _float fReturnLerp)
{
	m_pCineCamera->Play(strCinematic, strReturnName, iLerp, fReturnLerp);
}

HRESULT CCinematicCamera::Ready_PartObject()
{
	// CineCamera
	{
		CCineCamera::DESC CineCameraDesc{};

		CineCameraDesc.CameraDesc.GameObjectDesc.pParentWorldMatrix = &m_CombinedWorldMatrix;
		CineCameraDesc.CameraDesc.fFovY = XMConvertToRadians(60.f);
		CineCameraDesc.CameraDesc.fNear = 0.1f;
		CineCameraDesc.CameraDesc.fFar = 1000.f;

		CineCameraDesc.strName = L"Cinematic_Camera";
		CineCameraDesc.pOwner = this;

		if (FAILED(Add_PartObject(L"Part_CineCamera", LEVEL_STATIC, L"Prototype_GameObject_CineCamera", &CineCameraDesc)))
			return E_FAIL;

		m_pCineCamera = static_cast<CCineCamera*>(Find_PartObject({ L"Part_CineCamera" }));
		m_pCineCamera->Add_Cinematic(L"Client_Test", "../Bin/DataFiles/CameraData/Client_Test.data");
		m_pCineCamera->Add_Cinematic(L"Client_Test2", "../Bin/DataFiles/CameraData/Client_Test2.data");
	}

	return S_OK;
}

HRESULT CCinematicCamera::Ready_Event()
{
	// 테스트용 쉐이킹 이벤트
	if(FAILED(Add_Event(L"Event_Cinematic_Shake", CEvent_Cinematic_Shake::Create(this))))
		return E_FAIL;

	return S_OK;
}

CCinematicCamera* CCinematicCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicCamera* pInstance = new CCinematicCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCinematicCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCinematicCamera::Clone(void* pArg)
{
	CCinematicCamera* pInstance = new CCinematicCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCinematicCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinematicCamera::Free()
{
	super::Free();
}