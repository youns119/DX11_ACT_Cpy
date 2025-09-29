#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CCamera;

class CCameraManager final : public CBase
{
	using super = CBase;

public :
	enum class LERP
	{
		LERP_LINEAR,
		LERP_SINE,
		LERP_EASEIN,
		LERP_EASEOUT,
		LERP_EASEINOUT,

		LERP_END
	};

private:
	CCameraManager();
	virtual ~CCameraManager() = default;

public:
	void Priority_Update(_float fTimeDelta);

public :
	CCamera* Get_CurrCam() { return m_pCurrCam; }

	void Set_CurrCam(CCamera* pCamera);

public:
	void Add_Camera(const _wstring& strTag, CCamera* pCamera)
	{
		m_mapCamera.emplace(strTag, pCamera);
	}

public:
	HRESULT Camera_Transition(const _wstring& strDst, _uint iLerp, _float fLerpTime);

public :
	void Clear();

private:
	map<_wstring, CCamera*> m_mapCamera;

private:
	_bool m_bTransition{};

	CCamera* m_pCamSrc{};
	CCamera* m_pCamDst{};

	CCamera* m_pCurrCam{};

private :
	_float4x4 m_matSrc{};
	_float4x4 m_matTransition{};

	LERP m_eLerp{};
	_float m_fLerpAcc{};
	_float m_fLerp{};
	_float m_fEpsilon{ 0.05f };

	_float m_fFov{};
	_float m_fAspect{};
	_float m_fNear{};
	_float m_fFar{};

private :
	void Lerp_Camera(_float fTimeDelta);
	_float Lerp_Time();

private :
	CCamera* Find_Camera(const _wstring& strTag);

public:
	static CCameraManager* Create();
	virtual void Free() override;
};

END