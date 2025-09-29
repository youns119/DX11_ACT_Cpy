#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC GameObjectDesc {};

		_float3 vEye {};
		_float3 vAt {};
		_float fFovY { 0.f };

		_float fNear { 0.f };
		_float fFar { 0.f };
	};

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Get_FOV() const { return m_fFovY; }
	_float Get_Aspect() const { return m_fAspect; }
	_float Get_Near() const { return m_fNear; }
	_float Get_Far() const { return m_fFar; }

public:
	_bool Get_Active() { return m_bActive; }
	_bool IsCurrCam() const;

public:
	void Set_Active(_bool bActive) { m_bActive = bActive; }

protected:
	_float		m_fFovY { 0.f };
	_float		m_fNear { 0.f };
	_float		m_fFar { 0.f };
	_float		m_fAspect { 0.f };

	_bool m_bActive{};

protected:
	virtual void Set_Transform_To_PipeLine();

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END
