#include "CameraComponent.h"

#include "Camera.h"

CCameraComponent::CCameraComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
	, m_Gen(m_RD())
	, m_NoiseDist(-1.f, 1.f)
{
}

CCameraComponent::CCameraComponent(const CCameraComponent& Prototype)
	: super(Prototype)
{
}

HRESULT CCameraComponent::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraComponent::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	m_pOwner = pDesc->pOwner;

	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCameraComponent::Update(_float fTimeDelta)
{
	Shake(fTimeDelta);
}

void CCameraComponent::PositionShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop)
{
	m_tPositionShake.bShake = true;
	m_tPositionShake.bLoop = bLoop;
	m_tPositionShake.fAmplitude = fAmplitude;
	m_tPositionShake.fFrequency = fFrequency;
	m_tPositionShake.fDuration = fDuration;
	m_tPositionShake.fTimeElapsed = 0.f;

	if (m_tPositionShake.bLoop)
		m_tPositionShake.fDuration = FLT_MAX;
}

void CCameraComponent::RotationShake(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop)
{
	m_tRotationShake.bShake = true;
	m_tRotationShake.bLoop = bLoop;
	m_tRotationShake.fAmplitude = fAmplitude;
	m_tRotationShake.fFrequency = fFrequency;
	m_tRotationShake.fDuration = fDuration;
	m_tRotationShake.fTimeElapsed = 0.f;

	if (m_tPositionShake.bLoop)
		m_tPositionShake.fDuration = FLT_MAX;
}

void CCameraComponent::Stop_Shake()
{
	m_tPositionShake.bShake = false;
	m_tPositionShake.bLoop = false;
	m_tPositionShake.fAmplitude = 0.f;
	m_tPositionShake.fFrequency = 0.f;
	m_tPositionShake.fDuration = 0.f;
	m_tPositionShake.fTimeElapsed = 0.f;

	m_tRotationShake.bShake = false;
	m_tRotationShake.bLoop = false;
	m_tRotationShake.fAmplitude = 0.f;
	m_tRotationShake.fFrequency = 0.f;
	m_tRotationShake.fDuration = 0.f;
	m_tRotationShake.fTimeElapsed = 0.f;
}

void CCameraComponent::Shake(_float fTimeDelta)
{
	_float3 vShakePosition{};
	_float3 vShakeRotation{};

	// PositionShake
	if (m_tPositionShake.bShake && m_tPositionShake.fTimeElapsed < m_tPositionShake.fDuration)
	{
		_float fFadeFactor{ 1.f };
		if(!m_tPositionShake.bLoop)
			fFadeFactor = fFadeFactor - m_tPositionShake.fTimeElapsed / m_tPositionShake.fDuration;
		_float fFrequencyFactor = m_tPositionShake.fTimeElapsed * m_tPositionShake.fFrequency;

		_float fDistX = sinf(fFrequencyFactor + m_NoiseDist(m_Gen) * 2.f) * m_tPositionShake.fAmplitude * fFadeFactor;
		_float fDistY = cosf(fFrequencyFactor + m_NoiseDist(m_Gen) * 3.f) * m_tPositionShake.fAmplitude * fFadeFactor;
		_float fDistZ = sinf(fFrequencyFactor + m_NoiseDist(m_Gen) * 4.f) * m_tPositionShake.fAmplitude * fFadeFactor;

		vShakePosition.x += fDistX;
		vShakePosition.y += fDistY;
		vShakePosition.z += fDistZ;

		m_tPositionShake.fTimeElapsed += fTimeDelta;
	}
	else
	{
		m_tPositionShake.bShake = false;
		m_tPositionShake.vShakeResult = _float3(0.f, 0.f, 0.f);
	}

	// RotationShake
	if (m_tRotationShake.bShake && m_tRotationShake.fTimeElapsed < m_tRotationShake.fDuration)
	{
		_float fFadeFactor{ 1.f };
		if (!m_tRotationShake.bLoop)
			fFadeFactor = fFadeFactor - m_tRotationShake.fTimeElapsed / m_tRotationShake.fDuration;
		_float fFrequencyFactor = m_tRotationShake.fTimeElapsed * m_tRotationShake.fFrequency;

		_float fRotX = cosf(fFrequencyFactor + m_NoiseDist(m_Gen) * 2.f) * m_tRotationShake.fAmplitude * fFadeFactor;
		_float fRotY = sinf(fFrequencyFactor + m_NoiseDist(m_Gen) * 3.f) * m_tRotationShake.fAmplitude * fFadeFactor;
		_float fRotZ = cosf(fFrequencyFactor + m_NoiseDist(m_Gen) * 4.f) * m_tRotationShake.fAmplitude * fFadeFactor;

		vShakeRotation.x += fRotX;
		vShakeRotation.y += fRotY;
		vShakeRotation.z += fRotZ;

		m_tRotationShake.fTimeElapsed += fTimeDelta;
	}
	else
	{
		m_tRotationShake.bShake = false;
		m_tRotationShake.vShakeResult = _float3(0.f, 0.f, 0.f);
	}

	m_tPositionShake.vShakeResult = vShakePosition;
	m_tRotationShake.vShakeResult = vShakeRotation;
}

CCameraComponent* CCameraComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraComponent* pInstance = new CCameraComponent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCameraComponent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCameraComponent::Clone(void* pArg)
{
	CCameraComponent* pInstance = new CCameraComponent(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCameraComponent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraComponent::Free()
{
	super::Free();
}