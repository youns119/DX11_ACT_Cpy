#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CUI_Manager;

class CCamera_Free final : public CCamera
{
	using super = CCamera;

public:
	struct DESC
	{
		CCamera::DESC CameraDesc {};
		_float fMouseSensitivity { 1.f };
	};

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	void Set_Typing(_bool bTyping) { m_bTyping = bTyping; }

public:
	void ReLocate(_vec3 vPos, _vec3 vRot);

private:
	_bool m_bTyping{};

	_float m_fMouseSensitivity { 1.f };
	_float m_fSpeed{1.f};

private:
	CUI_Manager* m_pUIManager{ nullptr };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
