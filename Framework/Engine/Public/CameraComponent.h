#pragma once

#include "Component.h"

BEGIN(Engine)

class CCamera;

class ENGINE_DLL CCameraComponent final
	: public CComponent
{
	using super = CComponent;

public:
	struct DESC
	{
		CCamera* pOwner{};
	};

	struct SHAKE
	{
		_bool bShake{};
		_bool bLoop{};

		_float fAmplitude{};
		_float fFrequency{};
		_float fDuration{};
		_float fTimeElapsed{};

		_float3 vShakeResult{};
	};

private:
	CCameraComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraComponent(const CCameraComponent& Prototype);
	virtual ~CCameraComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta);

public:
	_bool Get_IsPositionShaking() { return m_tPositionShake.bShake; }
	_bool Get_IsRotationShaking() { return m_tRotationShake.bShake; }

	_float3 Get_ShakePosition() { return m_tPositionShake.vShakeResult; }
	_float3 Get_ShakeRotation() { return m_tRotationShake.vShakeResult; }

public:
	void PositionShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop = false);
	void RotationShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop = false);

public :
	void Stop_Shake();

private:
	CCamera* m_pOwner{};

private:
	SHAKE m_tPositionShake{};
	SHAKE m_tRotationShake{};

private:
	random_device m_RD;
	mt19937 m_Gen;
	uniform_real_distribution<_float> m_NoiseDist;

private:
	void Shake(_float fTimeDelta);

public:
	static CCameraComponent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END