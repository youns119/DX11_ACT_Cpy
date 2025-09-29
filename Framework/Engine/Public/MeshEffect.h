#pragma once
#include "EffectObject.h"

BEGIN(Engine)

class ENGINE_DLL CMeshEffect abstract : public CEffectObject
{
	using super = CEffectObject;

public:
	struct DESC
	{
		CEffectObject::DESC EffectDesc{};

		_wstring		strModelTag;
		_wstring		strTextureTag;
		_float3			vTranslation{};
		_float3			vScale{ 1.f, 1.f, 1.f };
		_uint			eLevelID;

		/* 메쉬 전용 */
		_bool			bIsLoop = {};
		_float3			vCenter = {};
		_float			fLifeTime = {};
		_uint			iMeshAnimOption = {};

		_bool			bUVAnim = {};
		_float2			vUVStart = {};
		_float2			vUVSpeed = {};
		_float2			vUVTileCount = {};

		/* 트레일 메쉬 전용 */
		_float			fMaxTrailLength = {};
	};

protected:
	CMeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshEffect(const CMeshEffect& Prototype);
	virtual ~CMeshEffect() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual HRESULT Save_Effect(HANDLE hFile, _ulong& dwByte) override;
	virtual void Clear_Info(_float fTimeDelta) override;

protected:
	_bool		m_bIsLoopEnd = {};
	/* 메쉬 */
	_bool		m_bIsLoop = {};
	_uint		m_iMeshAnimOption = {};
	_float		m_fLifeTime = {};
	_float		m_fLifeTimeAcc = {};
	_float3		m_vCenter = {};

	_bool		m_bUVAnim{};
	_float2		m_vUVStart{};
	_float2		m_vUVProgress{};
	_float2		m_vUVSpeed{};
	_float2		m_vUVTileCount{};
	_float		m_fMaxTrailLength{};

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END