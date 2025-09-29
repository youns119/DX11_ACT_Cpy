#pragma once
#include "EffectObject.h"
#include "VIBuffer_Instancing_Mesh_SRT.h"

BEGIN(Engine)

class ENGINE_DLL CMeshInstEffect abstract : public CEffectObject
{
	using super = CEffectObject;

public:
	struct DESC
	{
		_wstring		strTextureTag;
		CEffectObject::DESC EffectDesc{};
		CVIBuffer_Instancing_Mesh_SRT::DESC BufferDesc{};
		_float3			vOriginLocation{};
		_float			fLifeTime{};

		_bool			bUVAnim = {};
		_float2			vUVStart = {};
		_float2			vUVSpeed = {};
		_float2			vUVTileCount = {};
	};

protected:
	CMeshInstEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshInstEffect(const CMeshInstEffect& Prototype);
	virtual ~CMeshInstEffect() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual HRESULT Save_Effect(HANDLE hFile, _ulong& dwByte) override;

protected:
	_float		m_fLifeTime = {};
	_float		m_fLifeTimeAcc = {};

	_bool		m_bUVAnim{};
	_float2		m_vUVStart{};
	_float2		m_vUVProgress{};
	_float2		m_vUVSpeed{};
	_float2		m_vUVTileCount{};

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END