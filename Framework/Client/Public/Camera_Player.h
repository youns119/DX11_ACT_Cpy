#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class CCollider;
class CCameraComponent;

END

BEGIN(Client)

class CUI_Manager;

class CCamera_Player final
	: public CCamera
{
	using super = CCamera;

public:
	struct DESC
	{
		CCamera::DESC CameraDesc{};

		_float3 vOffset{};
	};

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& Prototype);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Get_IsFixed() const { return m_bCameraFix; }

public:
	void Camera_Fix();

public:
	void PositionShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop = false);
	void RotationShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop = false);

	void Stop_Shake();

public:
	const list<CGameObject*>& Get_CulledTargetList(const _wstring& LayerTag, _uint iCollision, list<CGameObject*>& listCulled, _fvector vPosOrigin);

public :
	void Calculate_CamTransform();

private:
	CCollider* m_pColliderCom{};
	CCameraComponent* m_pCameraCom{};

private:
	_matrix m_matOffset{};

private:
	_bool m_bCameraFix{ true };

private:
	CUI_Manager* m_pUIManager{ nullptr };

private:
	_bool m_bHasLight = { false };
private:
	virtual void Set_Transform_To_PipeLine();

private:
	HRESULT Ready_Component();

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END