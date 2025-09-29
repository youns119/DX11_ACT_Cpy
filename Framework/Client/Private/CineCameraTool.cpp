#include "pch.h"
#include "CineCameraTool.h"

#include "GameInstance.h"
#include "Collider.h"

#include "CineActorTool.h"

CCineCameraTool::CCineCameraTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCineCameraTool::CCineCameraTool(const CCineCameraTool& Prototype)
	: super(Prototype)
{
}

HRESULT CCineCameraTool::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCineCameraTool::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	pDesc->CameraDesc.vEye = _float3(7.f, 4.f, 7.f);
	pDesc->CameraDesc.vAt = _float3(0.f, 1.f, 0.f);
	pDesc->CameraDesc.fFovY = XMConvertToRadians(60.f);
	pDesc->CameraDesc.fNear = 0.1f;
	pDesc->CameraDesc.fFar = 1000.f;

	if (FAILED(super::Initialize(&pDesc->CameraDesc)) ||
		FAILED(Ready_Component()))
		return E_FAIL;

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	m_bColliderRender = false;

	m_pGameInstance->Add_Camera(L"Camera_Cine", this);

	return S_OK;
}

void CCineCameraTool::Priority_Update(_float fTimeDelta)
{
	_bool bFinished{};

	if (!m_bPlaying)
		return;
	else
	{
		m_fTimeAcc += fTimeDelta;
		bFinished = Proceed(m_fTimeAcc);
	}

	XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(&m_matTransition) * XMLoadFloat4x4(m_pParentWorldMatrix));
	Set_Transform_To_PipeLine();

	if (bFinished)
		m_pGameInstance->Camera_Transition(L"Camera_Free", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);

	super::Priority_Update(fTimeDelta);
}

void CCineCameraTool::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	for (auto& CineActor : m_listCineActor)
		CineActor->Update(fTimeDelta);

	super::Update(fTimeDelta);
}

void CCineCameraTool::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	if (m_bColliderRender)
	{
		// DebugComponent 등록
		_matrix matTransform = XMLoadFloat4x4(&m_CombinedWorldMatrix);

		_vec3 vPos = _mat(matTransform).position;
		_vec3 vLook = _mat(matTransform).look;
		_vec3 vRight = _mat(matTransform).right;
		_vec3 vUp = _mat(matTransform).up;

		m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::NONBLEND, m_pColliderCom);
		m_pGameInstance->Add_DebugRender_Ray(vPos, XMVector3Normalize(vLook._vector()) * 0.2f, false, _vec4(1.f, 0.f, 0.f, 0.f));
		m_pGameInstance->Add_DebugRender_Ray(vPos, XMVector3Normalize(vRight._vector()) * 0.2f, false, _vec4(0.f, 1.f, 0.f, 0.f));
		m_pGameInstance->Add_DebugRender_Ray(vPos, XMVector3Normalize(vUp._vector()) * 0.2f, false, _vec4(0.f, 0.f, 1.f, 0.f));
	}

	for (auto& CineActor : m_listCineActor)
		CineActor->Late_Update(fTimeDelta);

	if (m_bDrawPreview)
	{
		for (auto& Pair : m_listDraw)
			m_pGameInstance->Add_DebugRender_Ray(Pair.first, Pair.second, false, _vec4(1.f, 1.f, 0.f, 0.f));
	}

#endif

	super::Late_Update(fTimeDelta);
}

void CCineCameraTool::Final_Update(_float fTimeDelta)
{
	for (auto& CineActor : m_listCineActor)
		CineActor->Final_Update(fTimeDelta);

	super::Final_Update(fTimeDelta);
}

HRESULT CCineCameraTool::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	for (auto& CineActor : m_listCineActor)
		if (FAILED(CineActor->Render()))
			return E_FAIL;

	return S_OK;
}

void CCineCameraTool::Set_CurrTime(_float fTime)
{
	m_fTimeAcc = fTime;

	m_pCurrIter = m_listCineActor.begin();
	list<CCineActorTool*>::iterator pNextIter{};

	if (m_fTimeAcc >= m_fTotalDuration)
	{
		m_fTimeAcc = m_fTotalDuration;
		m_pCurrIter = prev(m_listCineActor.end());
		pNextIter = m_pCurrIter;
	}
	else
	{
		while (true)
		{
			if (m_fTimeAcc == 0.f)
				break;

			if ((*m_pCurrIter)->Get_CineActor()->fTime < m_fTimeAcc)
			{
				m_pCurrIter = next(m_pCurrIter);
				if ((*m_pCurrIter)->Get_CineActor()->fTime > m_fTimeAcc)
				{
					m_pCurrIter = prev(m_pCurrIter);
					break;
				}
				else if ((*m_pCurrIter)->Get_CineActor()->fTime == m_fTimeAcc)
					break;
			}
		}

		if (next(m_pCurrIter) == m_listCineActor.end())
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

	XMStoreFloat4x4(&m_matTransition, matLerp);

	{
		XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(&m_matTransition) * XMLoadFloat4x4(m_pParentWorldMatrix));

		if (m_bActive)
			Set_Transform_To_PipeLine();

		m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	}
}

void CCineCameraTool::Play(_bool bTransition)
{
	if (m_bPlaying)
	{
		Stop();
		return;
	}

	if (m_listCineActor.empty())
		return;

	m_bPlaying = true;
	if (m_pCurrIter == m_listCineActor.begin() && m_fTimeAcc == 0.f)
	{
		m_pCurrIter = m_listCineActor.begin();

		_matrix matTransition = (*m_pCurrIter)->Get_Transform()->Get_WorldMatrix();
		XMStoreFloat4x4(&m_matTransition, matTransition);
	}
	else
		Set_CurrTime(m_fTimeAcc);

	m_bColliderRender = true;

	if (bTransition)
	{
		m_pGameInstance->Camera_Transition(L"Camera_Cine", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.5f);
		m_bColliderRender = false;
	}
}

void CCineCameraTool::Stop()
{
	m_bPlaying = false;
}

HRESULT CCineCameraTool::Add_CineActor
(
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
	CCineActorTool::DESC tActorDesc{};
	tActorDesc.tCineActor.ePathType = PATHTYPE(iPathType);
	tActorDesc.tCineActor.fTime = fTime;
	tActorDesc.tCineActor.fLerp = fLerp;
	tActorDesc.tCineActor.eLerpType = LERPTYPE(iLerpType);
	tActorDesc.tCineActor.vBazierControl = vBazier;
	tActorDesc.tCineActor.vCatmullRomControlStart = vCatmullRomStart;
	tActorDesc.tCineActor.vCatmullRomControlEnd = vCatmullRomEnd;
	tActorDesc.vPosition = vPosition;
	tActorDesc.vRotation = vRotation;

	CCineActorTool* pCineActor = CCineActorTool::Create(m_pDevice, m_pContext);
	pCineActor->Initialize(&tActorDesc);

	m_listCineActor.push_back(pCineActor);

	Arrange_CineActorList();

	return S_OK;
}

HRESULT CCineCameraTool::Delete_CineActor(_string strName)
{
	for (auto iter = m_listCineActor.begin(); iter != m_listCineActor.end();)
	{
		if ((*iter)->Get_CineActor()->strName == strName)
		{
			Safe_Release(*iter);
			m_listCineActor.erase(iter);

			break;
		}
		else iter++;
	}

	Arrange_CineActorList();

	return S_OK;
}

void CCineCameraTool::Add_Notify(_float fTrackPosition, string& strNotifyName)
{
	m_queCamNotify.push(make_pair(fTrackPosition, strNotifyName));

	Arrange_Notify();
}

void CCineCameraTool::Change_Notify(string& strNotifyName, string& strChangeName, _float fTrackPosition, _float fChangePosition)
{
	queue<pair<_float, string>> queTemp = m_queCamNotify;

	while (!m_queCamNotify.empty())
		m_queCamNotify.pop();

	while (!queTemp.empty())
	{
		pair<_float, string> pairTemp = queTemp.front();
		queTemp.pop();

		if (pairTemp.first == fTrackPosition && pairTemp.second == strNotifyName)
			Add_Notify(fChangePosition, strChangeName);
		else Add_Notify(pairTemp.first, pairTemp.second);
	}
}

void CCineCameraTool::Delete_Notify(string& strNotifyName, _float fTrackPosition)
{
	queue<pair<_float, string>> queTemp;

	while (!m_queCamNotify.empty())
	{
		pair<_float, string> pairTemp = m_queCamNotify.front();
		m_queCamNotify.pop();

		if (pairTemp.first != fTrackPosition || pairTemp.second != strNotifyName)
			queTemp.push(pairTemp);
	}

	m_queCamNotify = queTemp;
}

void CCineCameraTool::Arrange_Notify()
{
	vector<pair<_float, string>> vecTemp;

	while (!m_queCamNotify.empty())
	{
		pair<_float, string> pairTemp = m_queCamNotify.front();
		m_queCamNotify.pop();

		vecTemp.push_back(pairTemp);
	}

	sort(vecTemp.begin(), vecTemp.end(), [](const auto& PrevPair, const auto& NextPair)
		{
			return PrevPair.first < NextPair.first;
		});

	for (auto& Pair : vecTemp)
		m_queCamNotify.push(Pair);
}

HRESULT CCineCameraTool::Save(HANDLE& hFile)
{
	_ulong dwByte{};

	WriteFile(hFile, &m_fTotalDuration, sizeof(_float), &dwByte, nullptr);

	_uint iSize = static_cast<_uint>(m_listCineActor.size());
	WriteFile(hFile, &iSize, sizeof(iSize), &dwByte, nullptr);

	for (auto& CineActor : m_listCineActor)
	{
		_uint iPathType = static_cast<_uint>(CineActor->Get_CineActor()->ePathType);
		WriteFile(hFile, &iPathType, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &CineActor->Get_CineActor()->fTime, sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &CineActor->Get_CineActor()->fLerp, sizeof(_float), &dwByte, nullptr);

		_uint iLerpType = static_cast<_uint>(CineActor->Get_CineActor()->eLerpType);
		WriteFile(hFile, &iLerpType, sizeof(_uint), &dwByte, nullptr);

		WriteFile(hFile, &CineActor->Get_CineActor()->vBazierControl, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &CineActor->Get_CineActor()->vCatmullRomControlStart, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &CineActor->Get_CineActor()->vCatmullRomControlEnd, sizeof(_float3), &dwByte, nullptr);

		CTransform* pTransform = CineActor->Get_Transform();
		_float3 vPosition = _vec3(pTransform->Get_State(CTransform::STATE::POSITION))._float3();
		_float3 vRotation = pTransform->Get_Rotation();
		WriteFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
	}

	_uint iCamNotifySize = static_cast<_uint>(m_queCamNotify.size());
	WriteFile(hFile, &iCamNotifySize, sizeof(_uint), &dwByte, nullptr);

	queue<pair<_float, string>> queTemp = m_queCamNotify;

	while (!queTemp.empty())
	{
		pair<_float, string> pairTemp = queTemp.front();
		queTemp.pop();

		_float fTrackPosition = pairTemp.first;
		const _char* szNotifyName = pairTemp.second.c_str();

		_char szNotify[MAX_PATH]{};
		strcpy_s(szNotify, szNotifyName);

		WriteFile(hFile, &fTrackPosition, sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &szNotify, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	}

	return S_OK;
}

HRESULT CCineCameraTool::Load(HANDLE& hFile)
{
	_bool bRead{};
	_ulong dwByte{};

	Clear();

	bRead = ReadFile(hFile, &m_fTotalDuration, sizeof(_float), &dwByte, nullptr);

	_uint iSize{};
	bRead = ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

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

		bRead = ReadFile(hFile, &iPathType, sizeof(_uint), &dwByte, nullptr);
		bRead = ReadFile(hFile, &fTime, sizeof(_float), &dwByte, nullptr);
		bRead = ReadFile(hFile, &fLerp, sizeof(_float), &dwByte, nullptr);

		bRead = ReadFile(hFile, &iLerpType, sizeof(_uint), &dwByte, nullptr);

		bRead = ReadFile(hFile, &vBazier, sizeof(_float3), &dwByte, nullptr);
		bRead = ReadFile(hFile, &vCatmullRomStart, sizeof(_float3), &dwByte, nullptr);
		bRead = ReadFile(hFile, &vCatmullRomEnd, sizeof(_float3), &dwByte, nullptr);

		bRead = ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
		bRead = ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);

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
	bRead = ReadFile(hFile, &iCamNotifySize, sizeof(_uint), &dwByte, nullptr);

	while (!m_queCamNotify.empty())
		m_queCamNotify.pop();

	for (_uint i = 0; i < iCamNotifySize; i++)
	{
		_float fTrackPosition{};
		_char szNotifyName[MAX_PATH]{};

		bRead = ReadFile(hFile, &fTrackPosition, sizeof(_float), &dwByte, nullptr);
		bRead = ReadFile(hFile, &szNotifyName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

		string strNotify = string(szNotifyName);

		Add_Notify(fTrackPosition, strNotify);
	}

	return S_OK;
}

void CCineCameraTool::Clear()
{
	for (auto& CineActor : m_listCineActor)
		Safe_Release(CineActor);

	m_listCineActor.clear();

	m_listDraw.clear();

	while(!m_queCamNotify.empty())
		m_queCamNotify.pop();

	m_bPlaying = false;
	m_fTimeAcc = 0.f;
	m_fPrevTime = 0.f;
	m_fTotalDuration = 0.f;
}

void CCineCameraTool::Arrange_CineActorList()
{
	if (m_listCineActor.empty())
		return;

	m_listCineActor.sort([](const auto& Prev, const auto& Post)
		{
			_float fPrevTime = Prev->Get_CineActor()->fTime;
			_float fPostTime = Post->Get_CineActor()->fTime;

			return fPrevTime < fPostTime;
		});

	_uint iNum{ 1 };
	for (auto& CineActor : m_listCineActor)
	{
		_string strName = "Cam" + to_string(iNum);

		CineActor->Get_CineActor()->strName = strName;
		iNum++;
	}

	m_pCurrIter = m_listCineActor.begin();
	m_fTotalDuration = m_listCineActor.back()->Get_CineActor()->fTime;

	Draw_Trajectory(m_fDrawDelta);
}

_bool CCineCameraTool::Proceed(_float fTimeAcc)
{
	_bool bFinished{};

	list<CCineActorTool*>::iterator pNextIter{};

	if (fTimeAcc >= m_fTotalDuration)
	{
		fTimeAcc = m_fTotalDuration;
		m_pCurrIter = prev(m_listCineActor.end());
		pNextIter = m_pCurrIter;

		bFinished = true;
	}
	else
	{
		if (next(m_pCurrIter) == m_listCineActor.end())
			pNextIter = m_pCurrIter;
		else if (fTimeAcc >= (*next(m_pCurrIter))->Get_CineActor()->fTime)
		{
			m_pCurrIter = next(m_pCurrIter);
			pNextIter = next(m_pCurrIter);
		}
		else pNextIter = next(m_pCurrIter);
	}

	_float fRatio{};
	_float fTimeDist = fTimeAcc - (*m_pCurrIter)->Get_CineActor()->fTime;
	_float fIterDist = (*pNextIter)->Get_CineActor()->fTime - (*m_pCurrIter)->Get_CineActor()->fTime;
	if (fIterDist == 0.f)
		fRatio = (fTimeAcc >= (*m_pCurrIter)->Get_CineActor()->fTime) ? 1.f : 0.f;
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
		m_pCurrIter = m_listCineActor.begin();

		m_bColliderRender = false;
	}

	return bFinished;
}

_float CCineCameraTool::LerpTime(LERPTYPE eLerpType, _float fDelta, _float fDuration)
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

_matrix CCineCameraTool::LinearLerp(CCineActorTool* pPrevActor, CCineActorTool* pPostActor, _float fRatio)
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

_matrix CCineCameraTool::BazierLerp(CCineActorTool* pPrevActor, CCineActorTool* pPostActor, _float fRatio)
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

_matrix CCineCameraTool::CatmullRomLerp(CCineActorTool* pPrevActor, CCineActorTool* pPostActor, _float fRatio)
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

void CCineCameraTool::Draw_Trajectory(_float fTimeDelta)
{
	m_listDraw.clear();

	_float fTimeAcc{};
	_float fTotalDuration = m_fTotalDuration;

	auto pCurrIter = m_listCineActor.begin();
	_matrix matCurrIter = (*pCurrIter)->Get_Transform()->Get_WorldMatrix();;

	while (fTimeAcc < fTotalDuration)
	{
		fTimeAcc += fTimeDelta;

		list<CCineActorTool*>::iterator pNextIter{};

		if (fTimeAcc >= fTotalDuration)
		{
			fTimeAcc = fTotalDuration;
			pCurrIter = prev(m_listCineActor.end());
			pNextIter = pCurrIter;
		}
		else
		{
			if (next(pCurrIter) == m_listCineActor.end())
				pNextIter = pCurrIter;
			else if (fTimeAcc >= (*next(pCurrIter))->Get_CineActor()->fTime)
			{
				pCurrIter = next(pCurrIter);
				pNextIter = next(pCurrIter);
			}
			else pNextIter = next(pCurrIter);
		}

		_float fRatio{};
		_float fTimeDist = fTimeAcc - (*pCurrIter)->Get_CineActor()->fTime;
		_float fIterDist = (*pNextIter)->Get_CineActor()->fTime - (*pCurrIter)->Get_CineActor()->fTime;
		if (fIterDist == 0.f)
			fRatio = 1.f;
		else fRatio = fTimeDist / fIterDist;

		_matrix matTransform{};

		PATHTYPE ePathType = (*pCurrIter)->Get_CineActor()->ePathType;
		if (ePathType == PATHTYPE::PATHTYPE_LINEAR)
			matTransform = LinearLerp(*pCurrIter, *pNextIter, fRatio);
		else if (ePathType == PATHTYPE::PATHTYPE_BAZIER)
			matTransform = BazierLerp(*pCurrIter, *pNextIter, fRatio);
		else if (ePathType == PATHTYPE::PATHTYPE_CATMULLROM)
			matTransform = CatmullRomLerp(*pCurrIter, *pNextIter, fRatio);

		_vec4 vPrevPos = _mat(matCurrIter).position;
		_vec4 vCurrPos = _mat(matTransform).position;
		_vec4 vLook = vCurrPos - vPrevPos;
		_float fDist = vLook.Length();

		_vector vOrigin = vPrevPos._vector();
		_vector vDir = XMVector3Normalize(vLook._vector()) * fDist;

		m_listDraw.push_back({ vOrigin, vDir });

		matCurrIter = matTransform;
	}
}

_matrix CCineCameraTool::Lerp_Camera(_fmatrix matLerp)
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

void CCineCameraTool::Set_Transform_To_PipeLine()
{
	if (!m_bActive)
		return;

	// 부모 매트릭스와의 매트릭스 계산
	_matrix matTransform = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pGameInstance->
		Set_Transform(CPipeLine::TRANSFORM::VIEW, matTransform);
	m_pGameInstance->
		Set_Transform(CPipeLine::TRANSFORM::PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));
}

HRESULT CCineCameraTool::Ready_Component()
{
	// Com_Collider
	{
		CCollider_OBB::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::DEFAULT;

		Desc.vExtents = { 0.1f, 0.1f, 0.1f };
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_OBB",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;

		m_pColliderCom->Set_DefaultColor(XMVectorSet(1.f, 0.f, 1.f, 1.f));
	}

	return S_OK;
}

CCineCameraTool* CCineCameraTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineCameraTool* pInstance = new CCineCameraTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCineCameraTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCineCameraTool::Clone(void* pArg)
{
	CCineCameraTool* pInstance = new CCineCameraTool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCineCameraTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCineCameraTool::Free()
{
	super::Free();

	Safe_Release(m_pColliderCom);

	Clear();
}