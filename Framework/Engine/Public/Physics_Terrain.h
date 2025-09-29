#pragma once

#include "Physics.h"
#include "Shared.h"

BEGIN(Engine)
class CPhysics_Mesh;
class CVIBuffer_Terrain;

/* Physics_Mesh 를 감싸고 관리하는 녀석 */
/* terrain을 나누고 cooked mesh로 만든 physics_Mesh를 들고 있어*/
class ENGINE_DLL CPhysics_Terrain final : public CComponent
{
	using super = CComponent;

public:
	struct DESC
	{
		CPhysics::DESC PhysicsDesc {};
		CVIBuffer_Terrain* pVIBuffer_Terrain { nullptr };
		_mat WorldMatrix {};
		_bool isInChunk { false };
	};

private:
	CPhysics_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysics_Terrain(const CPhysics_Terrain& Prototype);
	virtual ~CPhysics_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	HRESULT Initialize_Prototype(const _wstring& strFilePath, _uint iLevelID, const _wstring& strTerrainTag);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	PxPhysics&		m_Physics;
	PxScene*		m_pScene { nullptr };

	//CShared<vector<CPhysics_Mesh*>>* m_pSplitTerrainPhysicsPrototype { nullptr };
	vector<CPhysics_Mesh*> m_SplitTerrainPhysics;
	CShared<vector<_wstring>>* m_pPrototyeTags { nullptr };
	CShared<_uint>* m_pLevelID { nullptr };

private:
	vector<_uint> Create_Indices(const _uint iNumUnitVertices);

public:
	static CPhysics_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CPhysics_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFilePath, _uint iLevelID, const _wstring& strTerrainTag);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
