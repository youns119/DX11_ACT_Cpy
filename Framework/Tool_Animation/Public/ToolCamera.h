#pragma once

#include "ToolAnimation_Defines.h"
#include "Camera.h"

BEGIN(ToolAnimation)

class CToolCamera final
	: public CCamera
{
public:
	struct DESC
	{
		CCamera::DESC CameraDesc{};
		_float fMouseSensitivity{ 1.f };
	};

private:
	CToolCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CToolCamera(const CToolCamera& _Prototype);
	virtual ~CToolCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	void ReLocate(_vec3 vPos, _vec3 vRot);

private:
	_float m_fMouseSensitivity{};
	_float m_fCameraSpeed{ 1.f };

private:
	_float3 m_vInitialEye{};
	_float3 m_vInitialAt{};

private:
	_bool m_bSlow{};

public:
	static CToolCamera* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};

END