#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class CShader_Compute;
class ENGINE_DLL CVIBuffer_Instancing_Mesh_SRT final : public CVIBuffer
{
	using super = CVIBuffer;

public:
	enum MESH_PATTERN
	{
		FRACTION, EXPL_SLOWDOWN, 
		AROUND,
		RISE, HOLD,
		PATTERN_END
	};

public:
	struct DESC
	{
		_uint			eLevelID;
		_wstring		strModelTag;

		_uint			iPattern{};		// ��ǻƮ ���̴���

		_bool			bIsLoop{};

		_uint			iNumInstance{};	// ���� ����
		_float3			vRange{};		// ���� ����
		_float3			vCenter{};		// ���� ������
		_float3			vPivot{};		// �̵�(Translation) ���� �߽���

		_float2			vSpeed{};		// �� �޽� �̵� �ӵ� �ּ�, �ִ�(����)
		_float2			vLifeTime{};		// �ּ�, �ִ� ����(�ʱ�ȭ ���� y = 0, x = �ִ� ����)

		_float3			vScalingSize{};
		_float			fFinalSize{};			// �ּ�, �ִ� ũ��(����)

		_int			iRotationAxis{};
		_float			fRotationSpeed{};
		_float			fRotationFinalSpeed{};
	};

	struct CONST_BUFFER // 32
	{
		_int     iNumInstance{};		// 4
		_float   fTimeDelta{};			// 8
		_uint    bIsLoop{};				// 12
		_float   fPadding{};			// 16

		_float4  vPivot{};				// 16

		_float3  vOriginSize{};			// 12
		_float	fFinalSize{};			// 16
	};

private:
	CVIBuffer_Instancing_Mesh_SRT(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing_Mesh_SRT(const CVIBuffer_Instancing_Mesh_SRT& Prototype);
	virtual ~CVIBuffer_Instancing_Mesh_SRT() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_InputAssembler() override;
	virtual HRESULT Render() override;

public:
	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override { return FALSE; }

public:
	_bool	Progress(_float fTimeDelta);
	void	Change_Pattern(_uint iPattern);
	void	Reset_Life();

private:
	void	Create_CS(_uint iPattern);

private:
	const vector<VTXPARTICLE_MESH>& Get_ParticleInfo() const { return m_InstanceData; }

private:
	CShader_Compute*				m_pComputeShader{ nullptr };

	ID3D11Buffer*					m_pVBInstance{ nullptr };
	D3D11_BUFFER_DESC				m_InstanceBufferDesc{};
	D3D11_SUBRESOURCE_DATA			m_InstanceInitialData{};

	_uint							m_iInstancingDataStride{ 0 };
	_uint							m_iNumIndexPerInstance{ 0 };
	_uint							m_iNumInstance{ 0 };

	vector<VTXPARTICLE_MESH>		m_InstanceData;


	_uint							m_iPattern{};
	_float3							m_vPivot{};

	_bool							m_bBillboard{};	// �� �׳� ���� ��ǻƮ ���̴� �Լ��� �����..
	_bool							m_bIsLoop{};

	_float3							m_vRange{};		// ���� ����
	_float3							m_vCenter{};		// ���� ������

	_float2							m_vSpeed{};		// �� �޽� �̵� �ӵ� �ּ�, �ִ�(����)
	_float2							m_vLifeTime{};		// �ּ�, �ִ� ����(�ʱ�ȭ ���� y = 0, x = �ִ� ����)

	_float3							m_vScalingSize{};
	_float							m_fFinalSize{};			// �ּ�, �ִ� ũ��(����)

	_int							m_iRotationAxis{};
	_float							m_fRotationSpeed{};
	_float							m_fRotationFinalSpeed{};

	_float4x4						m_WorldMatrix{};
	_float4							m_vQuaternionRotation{};

	_bool							m_bIsRenderable{};

public:
	static  CVIBuffer_Instancing_Mesh_SRT* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


};

END