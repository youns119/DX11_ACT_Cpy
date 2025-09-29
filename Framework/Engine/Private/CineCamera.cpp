#include "CineCamera.h"

#include "GameInstance.h"
#include "CineActor.h"
#include "CameraComponent.h"

CCineCamera::CCineCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCineCamera::CCineCamera(const CCineCamera& Prototype)
	: super(Prototype)
{
}

HRESULT CCineCamera::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCineCamera::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	if (FAILED(super::Initialize(&pDesc->CameraDesc)) ||
		FAILED(Ready_Component()))
		return E_FAIL;

	m_pOwner = pDesc->pOwner;
	m_strName = pDesc->strName;

	m_pGameInstance->Add_Camera(m_strName, this);

	XMStoreFloat4x4(&m_matTransition, XMMatrixIdentity());

	return S_OK;
}

void CCineCamera::Priority_Update(_float fTimeDelta)
{
	if (m_bAutoPlay && m_bActive)
	{
		m_bPlaying = true;
		m_bAutoPlay = false;
	}

	_bool bFinished = Update_Camera(fTimeDelta);
	Update_CameraMatrix();
	Camera_Finish(bFinished);

	super::Priority_Update(fTimeDelta);
}

void CCineCamera::Update(_float fTimeDelta)
{
	if (m_bActive)
	{
		if(m_bPlaying)
			Call_AnimNotify();

		m_pCameraCom->Update(fTimeDelta);
	}

	for (auto& CineActor : m_listCurr)
		CineActor->Update(fTimeDelta);

	super::Update(fTimeDelta);
}

void CCineCamera::Late_Update(_float fTimeDelta)
{
	for (auto& CineActor : m_listCurr)
		CineActor->Late_Update(fTimeDelta);

	super::Late_Update(fTimeDelta);
}

void CCineCamera::Final_Update(_float fTimeDelta)
{
	for (auto& CineActor : m_listCurr)
		CineActor->Final_Update(fTimeDelta);

	super::Final_Update(fTimeDelta);
}

HRESULT CCineCamera::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	for (auto& CineActor : m_listCurr)
		if (FAILED(CineActor->Render()))
			return E_FAIL;

	return S_OK;
}

void CCineCamera::SetUp_CurrCinematic(const _wstring& strCinematic, const _wstring& strReturnName, _uint iLerp, _float fReturnLerp, _bool bAutoPlay)
{
	Play(strCinematic, strReturnName, iLerp, fReturnLerp);

	m_bPlaying = false;
	m_bAutoPlay = bAutoPlay;
}

void CCineCamera::Camera_LookAt(_fvector vPos)
{
	_vector vScale, vRotQuat, vTranslation;
	XMMatrixDecompose(&vScale, &vRotQuat, &vTranslation,
		XMLoadFloat4x4(&m_matTransition));

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_matrix matView = XMMatrixLookToLH(vTranslation, vPos, vUp);
	_matrix matWorld = XMMatrixInverse(nullptr, matView);

	//_matrix matRotView = XMMatrixLookToLH(vTranslation, vPos, vUp);
	//_matrix matRotWorld = XMMatrixInverse(nullptr, matRotView);
	//vRotQuat = XMQuaternionRotationMatrix(matRotWorld);

	//_matrix matView = XMMatrixLookAtLH(vTranslation, vPos, vUp);
	//_matrix matWorld = XMMatrixInverse(nullptr, matView);
	//vRotQuat = XMQuaternionRotationMatrix(matRotWorld);

	//_matrix matWorld =
	//	XMMatrixAffineTransformation(
	//		vScale,
	//		XMVectorZero(),
	//		vRotQuat,
	//		vTranslation
	//	);

	XMStoreFloat4x4(&m_matTransition, matWorld);
}

void CCineCamera::Play(const _wstring& strCinematic, const _wstring& strReturnName, _uint iLerp, _float fReturnLerp)
{
	auto iter = m_mapCineActor.find(strCinematic);
	if (iter == m_mapCineActor.end())
		return;

	if (m_mapCineActor[strCinematic].empty())
		return;

	m_strCinematic = strCinematic;
	m_listCurr = m_mapCineActor[strCinematic];

	m_bPlaying = true;
	m_pCurrIter = m_listCurr.begin();
	m_fTotalDuration = m_listCurr.back()->Get_CineActor()->fTime;

	m_strReturnName = strReturnName;
	m_iLerp = iLerp;
	m_fReturnLerp = fReturnLerp;

	XMStoreFloat4x4(&m_matTransition, (*m_pCurrIter)->Get_Transform()->Get_WorldMatrix());
}

void CCineCamera::Pause()
{
	m_bPlaying = false;
}

void CCineCamera::Resume()
{
	m_bPlaying = true;
}

void CCineCamera::Stop()
{
	m_bPlaying = false;
	m_bAutoPlay = false;
	m_fTimeAcc = 0.f;
	m_fPrevAcc = 0.f;
	m_pCurrIter = m_listCurr.begin();

	m_pGameInstance->Camera_Transition(m_strReturnName, m_iLerp, m_fReturnLerp);
}

void CCineCamera::PositionShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop)
{
	m_pCameraCom->PositionShake(fAmplitude, fFrequency, fDuration, bLoop);
}

void CCineCamera::RotationShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop)
{
	m_pCameraCom->RotationShake(fAmplitude, fFrequency, fDuration, bLoop);
}

void CCineCamera::Stop_Shake()
{
	m_pCameraCom->Stop_Shake();
}

HRESULT CCineCamera::Add_Cinematic(const _wstring& strCinematic, const _char* szFilePath)
{
	auto iter = m_mapCineActor.find(strCinematic);
	if (iter != m_mapCineActor.end())
		return E_FAIL;

	if (FAILED(Load(strCinematic, szFilePath)))
		return E_FAIL;

	return S_OK;
}

void CCineCamera::Add_Notify(const _wstring& strCinematic, _float fTrackPosition, string& strNotifyName)
{
	m_mapNotify[strCinematic].push(make_pair(fTrackPosition, strNotifyName));

	Arrange_Notify(strCinematic);
}

void CCineCamera::Arrange_Notify(const _wstring& strCinematic)
{
	vector<pair<_float, string>> vecTemp;

	while (!m_mapNotify[strCinematic].empty())
	{
		pair<_float, string> pairTemp = m_mapNotify[strCinematic].front();
		m_mapNotify[strCinematic].pop();

		vecTemp.push_back(pairTemp);
	}

	sort(vecTemp.begin(), vecTemp.end(), [](const auto& PrevPair, const auto& NextPair)
		{
			return PrevPair.first < NextPair.first;
		});

	for (auto& Pair : vecTemp)
		m_mapNotify[strCinematic].push(Pair);
}

_bool CCineCamera::Update_Camera(_float fTimeDelta)
{
	_bool bFinished{};

	if (m_bPlaying/* && m_bActive*/)
	{
		m_fPrevAcc = m_fTimeAcc;
		m_fTimeAcc += fTimeDelta;

		list<CCineActor*>::iterator pNextIter{};

		if (m_fTimeAcc >= m_fTotalDuration)
		{
			m_fTimeAcc = m_fTotalDuration;
			m_pCurrIter = prev(m_listCurr.end());
			pNextIter = m_pCurrIter;

			bFinished = true;
		}
		else
		{
			if (next(m_pCurrIter) == m_listCurr.end())
				pNextIter = m_pCurrIter;
			else if (m_fTimeAcc >= (*next(m_pCurrIter))->Get_CineActor()->fTime)
			{
				m_pCurrIter = next(m_pCurrIter);
				pNextIter = next(m_pCurrIter);
			}
			else pNextIter = next(m_pCurrIter);
		}

		_float fRatio{};
		_float fTimeDist = m_fTimeAcc - (*m_pCurrIter)->Get_CineActor()->fTime;
		_float fIterDist = (*pNextIter)->Get_CineActor()->fTime - (*m_pCurrIter)->Get_CineActor()->fTime;
		if (fIterDist == 0.f)
			fRatio = (m_fTimeAcc >= (*m_pCurrIter)->Get_CineActor()->fTime) ? 1.f : 0.f;
		else fRatio = LerpTime
		(
			(*m_pCurrIter)->Get_CineActor()->eLerpType,
			fTimeDist,
			fIterDist
		);

		_matrix matLerp{};
		PATHTYPE ePathType = (*m_pCurrIter)->Get_CineActor()->ePathType;
		if (ePathType == PATHTYPE::PATHTYPE_LINEAR)
			matLerp = LinearLerp(*m_pCurrIter, *pNextIter, fRatio);
		else if (ePathType == PATHTYPE::PATHTYPE_BAZIER)
			matLerp = BazierLerp(*m_pCurrIter, *pNextIter, fRatio);
		else if (ePathType == PATHTYPE::PATHTYPE_CATMULLROM)
			matLerp = CatmullRomLerp(*m_pCurrIter, *pNextIter, fRatio);

		XMStoreFloat4x4(&m_matTransition, Lerp_Camera(matLerp));

		if (bFinished)
		{
			m_bPlaying = false;
			m_fTimeAcc = 0.f;
			m_fPrevAcc = 0.f;
		}
	}

	return bFinished;
}

void CCineCamera::Update_CameraMatrix()
{
	if (m_pParentWorldMatrix == nullptr)
		XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(&m_matTransition));
	else XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(&m_matTransition) * XMLoadFloat4x4(m_pParentWorldMatrix));

	if (m_bActive)
	{
		Calculate_CamTransform();
		Set_Transform_To_PipeLine();
	}
}

void CCineCamera::Camera_Finish(_bool bFinished)
{
	if (bFinished)
	{
		m_pGameInstance->Camera_Transition(m_strReturnName, m_iLerp, m_fReturnLerp);
		m_pCurrIter = m_listCurr.begin();
		XMStoreFloat4x4(&m_matTransition, (*m_pCurrIter)->Get_Transform()->Get_WorldMatrix());
	}
}

HRESULT CCineCamera::Add_CineActor
(
	const _wstring& strCinematic,
	_uint iPathType, 
	_float fTime, 
	_float fLerp, 
	_uint iLerpType, 
	_float3 vPosition, 
	_float3 vRotation, 
	_float3 vBazier, 
	_float3 vCatmullRomStart, 
	_float3 vCatmullRomEnd
)
{
	CCineActor::DESC tActorDesc{};
	tActorDesc.tCineActor.ePathType = PATHTYPE(iPathType);
	tActorDesc.tCineActor.fTime = fTime;
	tActorDesc.tCineActor.fLerp = fLerp;
	tActorDesc.tCineActor.eLerpType = LERPTYPE(iLerpType);
	tActorDesc.tCineActor.vBazierControl = vBazier;
	tActorDesc.tCineActor.vCatmullRomControlStart = vCatmullRomStart;
	tActorDesc.tCineActor.vCatmullRomControlEnd = vCatmullRomEnd;
	tActorDesc.vPosition = vPosition;
	tActorDesc.vRotation = vRotation;

	CCineActor* pCineActor = CCineActor::Create(m_pDevice, m_pContext);
	pCineActor->Initialize(&tActorDesc);

	m_mapCineActor[strCinematic].push_back(pCineActor);

	Arrange_CineActorList(strCinematic);

	return S_OK;
}

void CCineCamera::Arrange_CineActorList(const _wstring& strCinematic)
{
	auto iter = m_mapCineActor.find(strCinematic);
	if (iter == m_mapCineActor.end())
		return;

	if (m_mapCineActor[strCinematic].empty())
		return;

	m_mapCineActor[strCinematic].sort([](const auto& Prev, const auto& Post)
		{
			_float fPrevTime = Prev->Get_CineActor()->fTime;
			_float fPostTime = Post->Get_CineActor()->fTime;

			return fPrevTime < fPostTime;
		});
}

_float CCineCamera::LerpTime(LERPTYPE eLerpType, _float fDelta, _float fDuration)
{
	_float fRatio{};

	switch ((_uint)eLerpType)
	{
	case (_uint)LERPTYPE::LERPTYPE_LINEAR:
		fRatio = fDelta / fDuration;
		break;
	case (_uint)LERPTYPE::LERPTYPE_ACCEL:
		fRatio = 1 + sinf(XMConvertToRadians(270.f + 90.f * (fDelta / fDuration)));
		break;
	case (_uint)LERPTYPE::LERPTYPE_DECEL:
		fRatio = sinf(XMConvertToRadians(90.f) * (fDelta / fDuration));
		break;
	case (_uint)LERPTYPE::LERPTYPE_SINE:
		fRatio = 0.5f + (sinf(XMConvertToRadians(-90.f + 180.f * (fDelta / fDuration))) * 0.5f);
		break;
	case (_uint)LERPTYPE::LERPTYPE_ARCSINE:
		fRatio = 0.5f + asinf(-1.f + 2.f * (fDelta / fDuration)) / (XM_PI * 0.5f) * 0.5f;
		break;
	default:
		break;
	}

	return fRatio;
}

_matrix CCineCamera::LinearLerp(CCineActor* pPrevActor, CCineActor* pPostActor, _float fRatio)
{
	_matrix matSrc = pPrevActor->Get_Transform()->Get_WorldMatrix();
	_matrix matDst = pPostActor->Get_Transform()->Get_WorldMatrix();

	_vector vSrcScale{}, vSrcRotation{}, vSrcTranslation{};
	_vector vDstScale{}, vDstRotation{}, vDstTranslation{};

	XMMatrixDecompose(&vSrcScale, &vSrcRotation, &vSrcTranslation, matSrc);
	XMMatrixDecompose(&vDstScale, &vDstRotation, &vDstTranslation, matDst);

	_vector vLerpScale = vSrcScale;
	_vector vLerpRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
	_vector vLerpTranslation = XMVectorSetW(XMVectorLerp(vSrcTranslation, vDstTranslation, fRatio), 1.f);

	_matrix matLerp =
		XMMatrixAffineTransformation(vLerpScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLerpRotation, vLerpTranslation);

	return matLerp;
}

_matrix CCineCamera::BazierLerp(CCineActor* pPrevActor, CCineActor* pPostActor, _float fRatio)
{
	_matrix matSrc = pPrevActor->Get_Transform()->Get_WorldMatrix();
	_matrix matDst = pPostActor->Get_Transform()->Get_WorldMatrix();

	_vector vSrcScale{}, vSrcRotation{}, vSrcTranslation{};
	_vector vDstScale{}, vDstRotation{}, vDstTranslation{};

	XMMatrixDecompose(&vSrcScale, &vSrcRotation, &vSrcTranslation, matSrc);
	XMMatrixDecompose(&vDstScale, &vDstRotation, &vDstTranslation, matDst);

	_vector vControl = _vec3(pPrevActor->Get_CineActor()->vBazierControl)._vector();
	_float fLerp = 1.f - fRatio;

	_vec3 vBazierA = XMVectorScale(vSrcTranslation, fLerp * fLerp);
	_vec3 vBazierB = XMVectorScale(vControl, 2.f * fRatio * fLerp);
	_vec3 vBazierC = XMVectorScale(vDstTranslation, fRatio * fRatio);
	_vector vLerpTranslation = XMVectorSetW((vBazierA + vBazierB + vBazierC)._vector(), 1.f);

	_vector vLerpScale = vSrcScale;
	_vector vLerpRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);

	_matrix matLerp =
		XMMatrixAffineTransformation(vLerpScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLerpRotation, vLerpTranslation);

	return matLerp;
}

_matrix CCineCamera::CatmullRomLerp(CCineActor* pPrevActor, CCineActor* pPostActor, _float fRatio)
{
	_matrix matSrc = pPrevActor->Get_Transform()->Get_WorldMatrix();
	_matrix matDst = pPostActor->Get_Transform()->Get_WorldMatrix();

	_vector vSrcScale{}, vSrcRotation{}, vSrcTranslation{};
	_vector vDstScale{}, vDstRotation{}, vDstTranslation{};

	XMMatrixDecompose(&vSrcScale, &vSrcRotation, &vSrcTranslation, matSrc);
	XMMatrixDecompose(&vDstScale, &vDstRotation, &vDstTranslation, matDst);

	_vector vPointA = _vec3(pPrevActor->Get_CineActor()->vCatmullRomControlStart)._vector();
	_vector vPointB = vSrcTranslation;
	_vector vPointC = vDstTranslation;
	_vector vPointD = _vec3(pPrevActor->Get_CineActor()->vCatmullRomControlEnd)._vector();

	_float fLerp1 = fRatio * fRatio;
	_float fLerp2 = fLerp1 * fRatio;

	_vector vCatmullA = XMVectorScale(vPointB, 2.f);
	_vector vCatmullB = XMVectorScale(XMVectorSubtract(vPointC, vPointA), fRatio);

	_vector vTempA = XMVectorSubtract(XMVectorScale(vPointA, 2.f), XMVectorScale(vPointB, 5.f));
	_vector vTempB = XMVectorSubtract(XMVectorScale(vPointC, 4.f), vPointD);
	_vector vCatmullC = XMVectorScale(XMVectorAdd(vTempA, vTempB), fLerp1);
	_vector vCatmullD =
		XMVectorScale(XMVectorAdd(XMVectorSubtract(XMVectorScale(vPointB, 3.f), XMVectorScale(vPointC, 3.f)),
			XMVectorAdd(XMVectorNegate(vPointA), vPointD)), fLerp2);

	_vector vFinalCatmull = XMVectorScale(XMVectorAdd(XMVectorAdd(vCatmullA, vCatmullB), XMVectorAdd(vCatmullC, vCatmullD)), 0.5f);
	_vector vLerpTranslation = XMVectorSetW(vFinalCatmull, 1.f);

	_vector vLerpScale = vSrcScale;
	_vector vLerpRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);

	_matrix matLerp =
		XMMatrixAffineTransformation(vLerpScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLerpRotation, vLerpTranslation);

	return matLerp;
}

_matrix CCineCamera::Lerp_Camera(_fmatrix matLerp)
{
	_matrix matSrc = XMLoadFloat4x4(&m_matTransition);

	_vector vSrcScale{}, vSrcRotation{}, vSrcTranslation{};
	_vector vLerpScale{}, vLerpRotation{}, vLerpTranslation{};

	XMMatrixDecompose(&vSrcScale, &vSrcRotation, &vSrcTranslation, matSrc);
	XMMatrixDecompose(&vLerpScale, &vLerpRotation, &vLerpTranslation, matLerp);

	vLerpRotation = XMQuaternionSlerp(vSrcRotation, vLerpRotation, (*m_pCurrIter)->Get_CineActor()->fLerp);
	vLerpTranslation = XMVectorSetW(XMVectorLerp(vSrcTranslation, vLerpTranslation, (*m_pCurrIter)->Get_CineActor()->fLerp), 1.f);

	_matrix matFinal = XMMatrixAffineTransformation(vLerpScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLerpRotation, vLerpTranslation);

	return matFinal;
}

void CCineCamera::Calculate_CamTransform()
{
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

void CCineCamera::Set_Transform_To_PipeLine()
{
	_matrix matTransform = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pGameInstance->
		Set_Transform(CPipeLine::TRANSFORM::VIEW, matTransform);
	m_pGameInstance->
		Set_Transform(CPipeLine::TRANSFORM::PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));
}

void CCineCamera::Call_AnimNotify()
{
	_wstring strCinematic = m_strCinematic;

	for (size_t i = 0; i < m_mapNotify[strCinematic].size(); i++)
	{
		pair<_float, string> pairFront = m_mapNotify[strCinematic].front();

		if (pairFront.first >= m_fPrevAcc &&
			pairFront.first <= m_fTimeAcc)
		{
			const _char* szNotifyName = pairFront.second.c_str();

			m_pOwner->Call_Event(szNotifyName);

			m_mapNotify[strCinematic].pop();
			m_mapNotify[strCinematic].push(pairFront);
		}
		else break;
	}
}

HRESULT CCineCamera::Load(const _wstring& strCinematic, const _char* szFilePath)
{
	_string strFilePath = _string(szFilePath);
	_wstring wstrFilePath = CUtils::ConvertToWideChar(strFilePath);

	_bool bResult{};
	_ulong dwByte{};
	HANDLE hFile{};
	hFile = CreateFile(wstrFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed To Load Camera Data");
		return E_FAIL;
	}

	bResult = ReadFile(hFile, &m_fTotalDuration, sizeof(_float), &dwByte, nullptr);

	_uint iSize{};
	bResult = ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iSize; i++)
	{
		_uint iPathType{};
		_float fTime{};
		_float fLerp{};
		_uint iLerpType{};
		_float3 vBazier{ 0.f, 0.f, 0.f };
		_float3 vCatmullRomStart{ 0.f, 0.f, 0.f };
		_float3 vCatmullRomEnd{ 0.f, 0.f, 0.f };
		_float3 vPosition{};
		_float3 vRotation{};

		bResult = ReadFile(hFile, &iPathType, sizeof(_uint), &dwByte, nullptr);
		bResult = ReadFile(hFile, &fTime, sizeof(_float), &dwByte, nullptr);
		bResult = ReadFile(hFile, &fLerp, sizeof(_float), &dwByte, nullptr);

		bResult = ReadFile(hFile, &iLerpType, sizeof(_uint), &dwByte, nullptr);

		bResult = ReadFile(hFile, &vBazier, sizeof(_float3), &dwByte, nullptr);
		bResult = ReadFile(hFile, &vCatmullRomStart, sizeof(_float3), &dwByte, nullptr);
		bResult = ReadFile(hFile, &vCatmullRomEnd, sizeof(_float3), &dwByte, nullptr);

		bResult = ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
		bResult = ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);

		if (iPathType == static_cast<_uint>(PATHTYPE::PATHTYPE_LINEAR))
		{
			vBazier = vPosition;
			vCatmullRomStart = vPosition;
			vCatmullRomEnd = vPosition;
		}
		else if (iPathType == static_cast<_uint>(PATHTYPE::PATHTYPE_BAZIER))
		{
			vCatmullRomStart = vPosition;
			vCatmullRomEnd = vPosition;
		}
		else if (iPathType == static_cast<_uint>(PATHTYPE::PATHTYPE_CATMULLROM))
		{
			vBazier = vPosition;
		}

		Add_CineActor
		(
			strCinematic, 
			iPathType,
			fTime,
			fLerp,
			iLerpType,
			vPosition,
			vRotation,
			vBazier,
			vCatmullRomStart,
			vCatmullRomEnd
		);
	}

	_uint iCamNotifySize{};
	bResult = ReadFile(hFile, &iCamNotifySize, sizeof(_uint), &dwByte, nullptr);
	
	while (!m_mapNotify[strCinematic].empty())
		m_mapNotify[strCinematic].pop();
	
	for (_uint i = 0; i < iCamNotifySize; i++)
	{
		_float fTrackPosition{};
		_char szNotifyName[MAX_PATH]{};
	
		bResult = ReadFile(hFile, &fTrackPosition, sizeof(_float), &dwByte, nullptr);
		bResult = ReadFile(hFile, &szNotifyName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	
		string strNotify = string(szNotifyName);
	
		Add_Notify(strCinematic, fTrackPosition, strNotify);
	}

	if (!bResult)
	{
		MSG_BOX("Failed To Load Camera Data");
		return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CCineCamera::Ready_Component()
{
	// Com_Camera
	{
		CCameraComponent::DESC Desc{};
		Desc.pOwner = this;

		m_pCameraCom = CCameraComponent::Create(m_pDevice, m_pContext);
		m_pCameraCom->Initialize(&Desc);
	}

	return S_OK;
}

CCineCamera* CCineCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineCamera* pInstance = new CCineCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCineCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCineCamera::Clone(void* pArg)
{
	CCineCamera* pInstance = new CCineCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCineCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCineCamera::Free()
{
	super::Free();

	for(auto& listActor : m_mapCineActor)
		for (auto& CineActor : listActor.second)
			Safe_Release(CineActor);

	m_mapCineActor.clear();

	Safe_Release(m_pCameraCom);
}