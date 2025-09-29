#pragma once

#include "GameObject.h"

#include "EPath.h"

BEGIN(Engine)

class CCameraDebug_Tool;

class ENGINE_DLL CCineActor
	: public CGameObject
{
	using super = CGameObject;

public:
	struct CINEACTOR
	{
		_string strName{};
		PATHTYPE ePathType{};

		_float fTime{};
		_float fLerp{ 1.f };

		LERPTYPE eLerpType{};

		_float3 vBazierControl{};

		_float3 vCatmullRomControlStart{};
		_float3 vCatmullRomControlEnd{};
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		_float3 vPosition{};
		_float3 vRotation{};

		CINEACTOR tCineActor{};
	};

protected:
	CCineActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCineActor(const CCineActor& Prototype);
	virtual ~CCineActor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CINEACTOR* Get_CineActor() { return &m_tCineActor; }
	CTransform* Get_Transform() { return m_pTransformCom; }

protected:
	CINEACTOR m_tCineActor{};

public:
	static CCineActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END