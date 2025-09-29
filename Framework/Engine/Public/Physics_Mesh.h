#pragma once

#include "Physics.h"
#include "Shared.h"

BEGIN(Engine)
class CModel;

class ENGINE_DLL CPhysics_Mesh final : public CPhysics
{
	using super = CPhysics;

public:
	struct DESC
	{
		super::DESC PhysicsDesc {};
		_mat WorldMatrix {};
		_bool isInChunk { false };
	};

private:
	CPhysics_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysics_Mesh(const CPhysics_Mesh& Prototype);
	virtual ~CPhysics_Mesh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	HRESULT Initialize_Prototype(const _wstring& strFilePath, _uint iLevelID, const _wstring& strModelTag);
	HRESULT Initialize_Prototype(const _wstring& strFilePath, const vector<vector<_vec3>>& Vertices, const vector<vector<_uint>>& Indices);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	CShared<vector<vector<_char>>>* m_pCookingData { nullptr };

#ifdef _DEBUG
	vector<vector<_vec3>> m_Positions;
	vector<vector<_uint>> m_Indices;

	_vec3 m_vMin {};
	_vec3 m_vMax {};
#endif // _DEBUG

private:
	HRESULT Write_TriangleMesh_To_File(HANDLE hFile, CModel* pModel);
	HRESULT Write_TriangleMesh_To_File(HANDLE hFile, const vector<vector<_vec3>>& Meshes, const vector<vector<_uint>>& MeshIndices);
	HRESULT Read_TriangleMesh_From_File(HANDLE hFile);


public:
	static CPhysics_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CPhysics_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFilePath, _uint iLevelID, const _wstring& strModelTag);
	static CPhysics_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFilePath, const vector<vector<_vec3>>& Vertices, const vector<vector<_uint>>& Indices);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
