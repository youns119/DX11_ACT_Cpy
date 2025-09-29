#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CEffectObject abstract : public CGameObject
{
	using super = CGameObject;

public:
	enum EFFECT_TYPE { PARTICLE, SPRITE, MESH, MESH_TRAIL, MESH_INSTANCE, TYPE_END };
	enum EFFECT_MESH_ANIM
	{
		ANIM_TRANS = 0x00000001,
		ANIM_ROTATE = 0x00000002,
		ANIM_SCALE = 0x00000004,
	};
	enum EFFECT_OPTION
	{
		MASK = 0x00000001,
		NOISE = 0x00000002,
		DISSOLVE = 0x00000004,
		EMISSIVE = 0x00000008,
		DISTORTION = 0x00000010,
	};
public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		_bool			bIsLoop = {};

		// 공통
		_bool			bBlendEnable = {};
		_float			fActiveDelay = {};
		_uint			iEffectType = {};
		_float			fFinalSize = {};
		_float3			vMulColor = {};
		_float3			vAddColor = {};

		// 쉐이딩 옵션
		_uint			iShadingOption = {};
		_uint			iMaskImageNum = {};
		_uint			iNoiseImageNum = {};
		_uint			iDissolveImageNum = {};
		_uint			iEmissiveImageNum = {};
		_uint			iDistortionImageNum = {};
		_float2			vDissolveStart = {};
		_float			fDistortionDissolveStart = {};
	};

protected:
	CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectObject(const CEffectObject& Prototype);
	virtual ~CEffectObject() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual HRESULT Save_Effect(HANDLE hFile, _ulong& dwByte);
	void	Set_AdoptedMatrix(const _float4x4* pAdoptedMatrix) { m_pAdoptedMatrix = pAdoptedMatrix; }
	void	Set_ParentWorldMatrix(const _float4x4* pParentWorldMatrix) { m_pParentWorldMatrix = pParentWorldMatrix; }
	virtual void Clear_Info(_float fTimeDelta);
	virtual _bool Call_Trigger_Event(_float fTimeDelta) { return true; }
/* 공용 */
protected:
	const _float4x4*  m_pAdoptedMatrix{ nullptr };

	_bool		m_bIsLoop = {};

	_bool		m_bBlendEnable = {};
	_float		m_fActiveDelay = {};
	_float		m_fActiveDelayAcc = {};

	_uint		m_iEffectType = {};
	_float		m_fFinalSize = {};
	_float3		m_vMulColor = {};
	_float3		m_vAddColor = {};

	_float4		m_vCamPosition = {};

/* 쉐이딩 옵션 */
protected:
	_uint		m_iShadingOption = {};
	_uint		m_iMaskNum{};
	_uint		m_iNoiseNum{};
	_uint		m_iDissolveNum{};
	_uint		m_iEmissiveNum{};
	_uint		m_iDistortionNum{};
	 
	_float2		m_vDissolveStart{};
	_float		m_fDistortionDissolveStart{};

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END