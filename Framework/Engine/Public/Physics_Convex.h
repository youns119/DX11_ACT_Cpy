#pragma once

#include "Physics.h"
#include "Shared.h"

BEGIN(Engine)
class CModel;

class ENGINE_DLL CPhysics_Convex final : public CPhysics
{
	using super = CPhysics;

public:
	struct DESC
	{
		super::DESC PhysicsDesc {};

		_bool isInChunk { false };
		_mat WorldMatrix {};
	};

private:
	CPhysics_Convex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysics_Convex(const CPhysics_Convex& Prototype);
	virtual ~CPhysics_Convex() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	HRESULT Initialize_Prototype(const _wstring& strFilePath, _uint iLevelID, const _wstring& strModelTag);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	CShared<vector<vector<_char>>>* m_pCookingData { nullptr };

#ifdef _DEBUG
	vector<PxVec3> m_Positions;
	vector<_uint> m_Indices;
#endif // _DEBUG

private:
	HRESULT Write_ConvexMesh_To_File(HANDLE hFile, CModel* pModel);
	HRESULT Read_ConvexMesh_From_File(HANDLE hFile);

public:
	static CPhysics_Convex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CPhysics_Convex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFilePath, _uint iLevelID, const _wstring& strModelTag);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
