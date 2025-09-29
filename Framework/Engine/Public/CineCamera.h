#pragma once

#include "Camera.h"

#include "EPath.h"

BEGIN(Engine)

class CCineActor;
class CCameraComponent;

class ENGINE_DLL CCineCamera
	: public CCamera
{
	using super = CCamera;

public:
	struct DESC
	{
		CCamera::DESC CameraDesc{};

		CGameObject* pOwner{};

		_wstring strName{};
	};

protected:
	CCineCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCineCamera(const CCineCamera& Prototype);
	virtual ~CCineCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Get_Playing() const { return m_bPlaying; }
	_float4x4 Get_TransitionMatrix() const { return m_matTransition; }

	void Set_TransitionMatrix(_fmatrix matTransition)
	{
		XMStoreFloat4x4(&m_matTransition, matTransition);
	}
	virtual void SetUp_CurrCinematic(const _wstring& strCinematic, const _wstring& strReturnName, _uint iLerp, _float fReturnLerp = 0.1f, _bool bAutoPlay = false);

public :
	void Camera_LookAt(_fvector vPos);

public:
	virtual void Play(const _wstring& strCinematic, const _wstring& strReturnName, _uint iLerp, _float fReturnLerp = 0.1f);
	virtual void Pause();
	virtual void Resume();
	virtual void Stop();

public:
	virtual void PositionShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop = false);
	virtual void RotationShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop = false);

	virtual void Stop_Shake();

public :
	virtual HRESULT Add_Cinematic(const _wstring& strCinematic, const _char* szFilePath);

public:
	void Add_Notify(const _wstring& strCinematic, _float fTrackPosition, string& strNotifyName);
	void Arrange_Notify(const _wstring& strCinematic);

protected:
	_wstring m_strName{};
	_wstring m_strCinematic{};

protected:
	map<_wstring, list<CCineActor*>> m_mapCineActor;
	list<CCineActor*> m_listCurr;
	list<CCineActor*>::iterator m_pCurrIter{};

protected :
	map<_wstring, queue<pair<_float, string>>> m_mapNotify;

protected :
	CGameObject* m_pOwner{};

protected:
	_bool m_bPlaying{};
	_bool m_bAutoPlay{};
	_float m_fTimeAcc{};
	_float m_fPrevAcc{};
	_float m_fTotalDuration{};

protected:
	_float4x4 m_matTransition{};

protected:
	_wstring m_strReturnName{};
	_uint m_iLerp{};
	_float m_fReturnLerp{};

protected:
	CCameraComponent* m_pCameraCom{};

protected :
	virtual _bool Update_Camera(_float fTimeDelta);
	virtual void Update_CameraMatrix();
	virtual void Camera_Finish(_bool bFinished);

protected:
	virtual HRESULT Add_CineActor
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
	);
	virtual void Arrange_CineActorList(const _wstring& strCinematic);

protected :
	virtual _float LerpTime(LERPTYPE eLerpType, _float fDelta, _float fDuration);

protected:
	virtual _matrix LinearLerp(CCineActor* pPrevActor, CCineActor* pPostActor, _float fRatio);
	virtual _matrix BazierLerp(CCineActor* pPrevActor, CCineActor* pPostActor, _float fRatio);
	virtual _matrix	CatmullRomLerp(CCineActor* pPrevActor, CCineActor* pPostActor, _float fRatio);

protected:
	virtual _matrix Lerp_Camera(_fmatrix matLerp);

protected:
	virtual void Calculate_CamTransform();
	virtual void Set_Transform_To_PipeLine() override;

protected:
	virtual void Call_AnimNotify();

protected:
	virtual HRESULT Load(const _wstring& strCinematic, const _char* szFilePath);

protected:
	virtual HRESULT Ready_Component();

public:
	static CCineCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END