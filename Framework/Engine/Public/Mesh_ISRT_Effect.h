#pragma once
#include "EffectObject.h"
#include "VIBuffer_Instancing_Mesh_SRT.h"

BEGIN(Engine)

class ENGINE_DLL CMesh_ISRT_Effect abstract : public CEffectObject
{
	using super = CEffectObject;

public:
	struct DESC
	{
		CEffectObject::DESC EffectDesc{};
		CVIBuffer_Instancing_Mesh_SRT::DESC BufferDesc{};

		_wstring		strTextureTag;
		_uint			eLevelID;

		_float			fMaxLifeTime;

		/* 파티클 & 스프라이트 전용 */
		_int			iPattern = {};

		/* 스프라이트 전용 */
		_int			iNumAtlas = {};
		_int2			vNumAtlas = {};
		_float2			vUVInterval = {}; 
	};

protected:
	CMesh_ISRT_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_ISRT_Effect(const CMesh_ISRT_Effect& Prototype);
	virtual ~CMesh_ISRT_Effect() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual HRESULT Save_Effect(HANDLE hFile, _ulong& dwByte) override;
	virtual void	Clear_Info(_float fTimeDelta) override;

protected:
	/* 파티클 & 스프라이트 */
	_float		m_fMaxLifeTime = {};
	_float		m_fLifeTimeAcc = {};

	_int		m_iPattern = {};
	_int3		m_vNumAtlas = {};
	_float2		m_vUVInterval = {};

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END