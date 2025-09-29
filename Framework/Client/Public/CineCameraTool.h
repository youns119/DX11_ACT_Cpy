#pragma once

#include "Client_Defines.h"

#include "Camera.h"

#include "EPath.h"

BEGIN(Client)

class CCineActorTool;

class CCineCameraTool final
	: public CCamera
{
	using super = CCamera;

public:
	struct DESC
	{
		CCamera::DESC CameraDesc{};
	};

private:
	CCineCameraTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCineCameraTool(const CCineCameraTool& Prototype);
	virtual ~CCineCameraTool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Get_CurrTime() const { return m_fTimeAcc; }
	_float Get_Duration() const { return m_fTotalDuration; }

	_float Get_DrawDelta() const { return m_fDrawDelta; }

	list<CCineActorTool*>* Get_CineActorList() { return &m_listCineActor; }

	queue<pair<_float, string>> Get_CamNotify() { return m_queCamNotify; }

public:
	void Set_CurrTime(_float fTime);
	void Set_Duration(_float fDuration) { m_fTotalDuration = fDuration; }

	void Set_Draw(_bool bDraw) { m_bDrawPreview = bDraw; }
	void Set_DrawDelta(_float fDrawDelta) { m_fDrawDelta = fDrawDelta; }

public:
	void Play(_bool bTransition);
	void Stop();

public:
	HRESULT Add_CineActor
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
	);
	HRESULT Delete_CineActor(_string strName);

public:
	void Add_Notify(_float fTrackPosition, string& strNotifyName);
	void Change_Notify(string& strNotifyName, string& strChangeName, _float fTrackPosition, _float fChangePosition);
	void Delete_Notify(string& strNotifyName, _float fTrackPosition);

	void Arrange_Notify();

public:
	HRESULT Save(HANDLE& hFile);
	HRESULT Load(HANDLE& hFile);

	void Clear();

public:
	void Arrange_CineActorList();

private:
	list<CCineActorTool*> m_listCineActor;
	list<CCineActorTool*>::iterator m_pCurrIter{};

private:
	_bool m_bPlaying{};
	_float m_fTimeAcc{};
	_float m_fPrevTime{};
	_float m_fTotalDuration{};

private:
	_float4x4 m_matTransition{};

private:
	CCollider* m_pColliderCom{};

	_bool m_bColliderRender{};

private:
	_bool m_bDrawPreview{};
	_float m_fDrawDelta{ 0.01f };

	list<pair<_vector, _vector>> m_listDraw;

private :
	queue<pair<_float, string>> m_queCamNotify;

private :
	_bool Proceed(_float fTimeAcc);

private:
	_float LerpTime(LERPTYPE eLerpType, _float fDelta, _float fDuration);

private:
	_matrix LinearLerp(CCineActorTool* pPrevActor, CCineActorTool* pPostActor, _float fRatio);
	_matrix BazierLerp(CCineActorTool* pPrevActor, CCineActorTool* pPostActor, _float fRatio);
	_matrix	CatmullRomLerp(CCineActorTool* pPrevActor, CCineActorTool* pPostActor, _float fRatio);

private:
	void Draw_Trajectory(_float fTimeDelta);

private:
	_matrix Lerp_Camera(_fmatrix matLerp);

private:
	virtual void Set_Transform_To_PipeLine() override;

private:
	HRESULT Ready_Component();

public:
	static CCineCameraTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END