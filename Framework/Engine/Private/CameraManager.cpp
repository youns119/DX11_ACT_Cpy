#include "CameraManager.h"

#include "GameInstance.h"
#include "Camera.h"

CCameraManager::CCameraManager()
{
}

void CCameraManager::Priority_Update(_float fTimeDelta)
{
	if (m_bTransition)
	{
		Lerp_Camera(fTimeDelta);

		_matrix matTransform = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matTransition));

		CGameInstance::GetInstance()->
			Set_Transform(CPipeLine::TRANSFORM::VIEW, matTransform);
		CGameInstance::GetInstance()->
			Set_Transform(CPipeLine::TRANSFORM::PROJ, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));
	}
}

void CCameraManager::Set_CurrCam(CCamera* pCamera)
{
	m_pCurrCam = pCamera;

	m_pCamSrc = pCamera;
	m_pCamSrc->Set_Active(true);
}

HRESULT CCameraManager::Camera_Transition(const _wstring& strDst, _uint iLerp, _float fLerpTime)
{
	m_pCamDst = Find_Camera(strDst);

	if (m_pCamSrc == nullptr || m_pCamDst == nullptr)
		return E_FAIL;

	m_pCamSrc->Set_Active(false);

	if (m_fLerpAcc > 0.f)
		m_matSrc = m_matTransition;
	else
		m_matSrc = m_pCamSrc->Get_CombinedMatrix();

	if (fLerpTime <= 0.f)
	{
		Set_CurrCam(m_pCamDst);
		m_bTransition = false;

		return S_OK;
	}
	else
		m_bTransition = true;

	m_pCurrCam = m_pCamDst;

	m_eLerp = (LERP)iLerp;
	m_fLerp = fLerpTime;
	m_fLerpAcc = 0.f;

	return S_OK;
}

void CCameraManager::Clear()
{
	m_mapCamera.clear();
}

void CCameraManager::Lerp_Camera(_float fTimeDelta)
{
	//m_fLerpAcc += fTimeDelta / m_fLerp;
	//if (m_fLerpAcc >= 1.f)
	//	m_fLerpAcc = 1.f;

	m_fLerpAcc *= m_fLerp;
	m_fLerpAcc += fTimeDelta;
	m_fLerpAcc /= m_fLerp;
	if (m_fLerpAcc >= 1.f)
		m_fLerpAcc = 1.f;

	_float fLerp = Lerp_Time();

	_matrix matSrc = XMLoadFloat4x4(&m_matSrc);
	_matrix matDst = XMLoadFloat4x4(&m_pCamDst->Get_CombinedMatrix());

	_vector vSrcScale{}, vSrcRotation{}, vSrcTranslation{};
	_vector vDstScale{}, vDstRotation{}, vDstTranslation{};

	XMMatrixDecompose(&vSrcScale, &vSrcRotation, &vSrcTranslation, matSrc);
	XMMatrixDecompose(&vDstScale, &vDstRotation, &vDstTranslation, matDst);

	_vector vLerpScale = vSrcScale;
	_vector vLerpRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fLerp);
	_vector vLerpTranslation = XMVectorSetW(XMVectorLerp(vSrcTranslation, vDstTranslation, fLerp), 1.f);

	_matrix matLerp =
		XMMatrixAffineTransformation(vLerpScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLerpRotation, vLerpTranslation);

	XMStoreFloat4x4(&m_matTransition, matLerp);

	m_fFov = CUtils::Lerp_Float(m_pCamSrc->Get_FOV(), m_pCamDst->Get_FOV(), fLerp);
	m_fAspect = CUtils::Lerp_Float(m_pCamSrc->Get_Aspect(), m_pCamDst->Get_Aspect(), fLerp);
	m_fNear = CUtils::Lerp_Float(m_pCamSrc->Get_Near(), m_pCamDst->Get_Near(), fLerp);
	m_fFar = CUtils::Lerp_Float(m_pCamSrc->Get_Far(), m_pCamDst->Get_Far(), fLerp);

	if (m_fLerpAcc >= 1.f)
	{
		m_fLerpAcc = 0.f;
		m_bTransition = false;
		Set_CurrCam(m_pCamDst);
	}
}

_float CCameraManager::Lerp_Time()
{
	_float fLerp = m_fLerpAcc;

	_uint iType = static_cast<_uint>(m_eLerp);
	switch (iType)
	{
	case (_uint)LERP::LERP_LINEAR:
	{
		//fLerp = m_fLerpAcc / 1.f;
		fLerp = m_fLerpAcc;
	}
	break;
	case (_uint)LERP::LERP_SINE:
	{
		//fLerp = sinf(XMConvertToRadians(90.f * (m_fLerpAcc / 1.f)));
		fLerp = sinf(XMConvertToRadians(90.f * fLerp));
	}
	break;
	case (_uint)LERP::LERP_EASEIN:
	{
		//fLerp = m_fLerpAcc / 1.f;
		fLerp = (fLerp == 0.f) ? 0.f : powf(2.f, 10.f * (fLerp - 1.f));
	}
	break;
	case (_uint)LERP::LERP_EASEOUT:
	{
		//fLerp = m_fLerpAcc / 1.f;
		fLerp = (fLerp == 1.f) ? 1.f : 1.f - powf(2.f, -10.f * fLerp);
	}
	break;
	case (_uint)LERP::LERP_EASEINOUT:
	{
		//fLerp = m_fLerpAcc / 1.f;

		if (fLerp < 0.5f)
			fLerp = 8.f * fLerp * fLerp * fLerp * fLerp;
		else 
		{
			fLerp = fLerp - 1.f;
			fLerp = 1.f - 8.f * fLerp * fLerp * fLerp * fLerp;
		}
	}
	break;
	default :
		break;
	}

	return fLerp;
}

CCamera* CCameraManager::Find_Camera(const _wstring& strTag)
{
	auto iter = m_mapCamera.find(strTag);
	if (iter == m_mapCamera.end())
		return nullptr;

	return iter->second;
}

CCameraManager* CCameraManager::Create()
{
	return new CCameraManager;
}

void CCameraManager::Free()
{
	super::Free();

	m_mapCamera.clear();
}