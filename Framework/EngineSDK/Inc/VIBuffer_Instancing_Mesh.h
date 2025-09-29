#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing_Mesh final : public CVIBuffer
{
	using super = CVIBuffer;

public:
	struct DESC
	{
		_uint			iTotalInstance = {};
		_uint			iNumInstance = {};
		_float3			vRange = {};
		_float3			vCenter = {};
		_float2			vSize = {};
		_float2			vRotation = {}; // Degree

	};

private:
	CVIBuffer_Instancing_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing_Mesh(const CVIBuffer_Instancing_Mesh& Prototype);
	virtual ~CVIBuffer_Instancing_Mesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const DESC* pDesc, const _wstring& strPrototypeTag, const _uint iLevelIndex);
	virtual HRESULT Initialize_Prototype(const DESC* pDesc, const _wstring& strPrototypeTag, const _uint iLevelIndex, const _wstring& strInstancingFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_InputAssembler() override;
	virtual HRESULT Render() override;

	void Input_InstancingInfo(const _int& iNumInstancing, const _float3& vCenter, const _float3& vRange, const _float2& vSize);
	void Delete_PrevInstancing(const _int& iNumInstancing);
	void Save_InstancingFile(HANDLE hFile);

	_bool Find_PickedInstance_WorldMatrix(const _float3& vPickedPos, _float4x4& OutMatrix);
	_float4x4 CombineToWorldMatrix(const _float4& vLight, const _float4& vUp, const _float4& vLook, const _float4& vTranslation);
	_bool Get_PickInstanceMesh_WorldMatrix(_float4x4& OutMatrix);
	_bool Set_PickInstanceMesh_WorldMatrix(_float4x4& InMatrix);

	virtual _bool Ray_Cast(_fvector vRayOrigin, _fvector vRayDirection, _float& fDist) override { return FALSE; }

private:
	const vector<VTXINSTANCE>& Get_ParticleInfo() const { return m_InstanceVertices; }

private:
	ID3D11Buffer* m_pVBInstance{ nullptr };
	D3D11_BUFFER_DESC				m_InstanceBufferDesc{};
	D3D11_SUBRESOURCE_DATA			m_InstanceInitialData{};

	_uint							m_iInstanceVertexStride{ 0 };
	_uint							m_iNumIndexPerInstance{ 0 };
	_uint							m_iNumInstance{ 0 };
	_uint							m_iTotalInstance{ 0 };

	_int							m_iPickedInstanceIdx = { -1 };
	vector<VTXINSTANCE>				m_InstanceVertices;

	vector<_float3>					m_Positions;
	//_float3						m_vPivot{};

	//vector<_float>				m_Sizes; // 필여 없을거 같기더....ㅇㅁㅇ   
public:
	static  CVIBuffer_Instancing_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const DESC* pDesc, const _wstring& strPrototypeTag, const _uint iLevelIndex);
	static  CVIBuffer_Instancing_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const DESC* pDesc, const _wstring& strPrototypeTag, const _uint iLevelIndex, const _wstring& strInstancingFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


};

END