#include "Physics_Terrain.h"

#include "GameInstance.h"

CPhysics_Terrain::CPhysics_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
	, m_Physics { PxGetPhysics() }
{
}

CPhysics_Terrain::CPhysics_Terrain(const CPhysics_Terrain& Prototype)
    : super(Prototype)
	, m_Physics { PxGetPhysics() }
	//, m_pSplitTerrainPhysicsPrototype { m_pSplitTerrainPhysicsPrototype }
	, m_pPrototyeTags { Prototype.m_pPrototyeTags }
	, m_pLevelID { Prototype.m_pLevelID }
{
	m_Physics.getScenes(&m_pScene, 1);
	Safe_AddRef(m_pPrototyeTags);
	Safe_AddRef(m_pLevelID);
	//Safe_AddRef(m_pSplitTerrainPhysicsPrototype);
}

HRESULT CPhysics_Terrain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPhysics_Terrain::Initialize_Prototype(const _wstring& strFilePath, _uint iLevelID, const _wstring& strTerrainTag)
{
	m_pPrototyeTags = CShared<vector<_wstring>>::Create();
	m_pLevelID = CShared<_uint>::Create();
	m_pLevelID->Value() = iLevelID;

	_wstring strDrive, strDirectory, strFileName, strEXT;
	CUtils::Split_Path(strFilePath, strDrive, strDirectory, strFileName, strEXT);

	CVIBuffer_Terrain* pVIBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Clone_Prototype(iLevelID, strTerrainTag));
	const vector<VTXNORTEX>& Vertices = pVIBuffer_Terrain->Get_VertexData();
	const vector<_uint>& Indices = pVIBuffer_Terrain->Get_IndexData();

	const _uint iNumUnitVertices = 32;
	_uint iNumWidthVertices = pVIBuffer_Terrain->Get_NumWidthVertices();
	_uint iNumDepthVertices = pVIBuffer_Terrain->Get_NumDepthVertices();


	for (_uint iOffsetZ = 0; iOffsetZ < (iNumDepthVertices - 1) / iNumUnitVertices; ++iOffsetZ)
	{
		for (_uint iOffsetX = 0; iOffsetX < (iNumWidthVertices - 1) / iNumUnitVertices; ++iOffsetX)
		{
			vector<_vec3> SplitTerrain;
			SplitTerrain.reserve(iNumUnitVertices * iNumUnitVertices);

			_uint iOffsetIndex = (iOffsetZ * iNumUnitVertices) * iNumWidthVertices + (iOffsetX * iNumUnitVertices);

			for (_uint z = 0; z <= iNumUnitVertices; ++z)
			{
				for (_uint x = 0; x <= iNumUnitVertices; ++x)
				{
					_uint iIndex = iOffsetIndex + z * iNumWidthVertices + x;

					_vec3 vPosition = Vertices[iIndex].vPosition;
					SplitTerrain.push_back(vPosition);
				}
			}
			vector<vector<_vec3>> SplitTerrains;
			SplitTerrains.push_back(SplitTerrain);
			vector<vector<_uint>> Indices;
			Indices.push_back(Create_Indices(iNumUnitVertices));

			//_wstring strIndexedFilePath = strDrive + strDirectory + strFileName + to_wstring(m_pSplitTerrainPhysicsPrototype->Value().size()) + strEXT;
			//m_pSplitTerrainPhysicsPrototype->Value().push_back(CPhysics_Mesh::Create(m_pDevice, m_pContext, strIndexedFilePath, SplitTerrains, Indices));

			_wstring strIndexedFilePath = strDrive + strDirectory + strFileName + to_wstring(m_pPrototyeTags->Value().size()) + strEXT;
			_wstring strIndexedPrototypeTag = L"Prototype_Component_Physics_Mesh_" + strFileName + to_wstring(m_pPrototyeTags->Value().size());
			m_pGameInstance->Add_Prototype(iLevelID, strIndexedPrototypeTag,
				CPhysics_Mesh::Create(m_pDevice, m_pContext, strIndexedFilePath, SplitTerrains, Indices));

			m_pPrototyeTags->Value().push_back(strIndexedPrototypeTag);
		}
	}

	Safe_Release(pVIBuffer_Terrain);
	return S_OK;
}

HRESULT CPhysics_Terrain::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	const _mat& WorldMatrix = pDesc->WorldMatrix;

	CPhysics_Mesh::DESC PhysicsMeshDesc {};
	PhysicsMeshDesc.PhysicsDesc = pDesc->PhysicsDesc;
	PhysicsMeshDesc.isInChunk = true;
	PhysicsMeshDesc.WorldMatrix = WorldMatrix;


	for (size_t i =0; i<m_pPrototyeTags->Value().size(); ++i)
	{
		m_SplitTerrainPhysics.push_back(static_cast<CPhysics_Mesh*>(m_pGameInstance->
			Clone_Prototype(m_pLevelID->Value(), m_pPrototyeTags->Value()[i], &PhysicsMeshDesc)));

		if (nullptr == m_SplitTerrainPhysics.back())
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPhysics_Terrain::Render()
{
#ifdef _DEBUG
	for (auto& pPhysics : m_SplitTerrainPhysics)
	{
		//if (pPhysics->Get_Count() > 0)
		if (pPhysics->Is_Simulated())
			pPhysics->Render();
	}
#endif // _DEBUG
	return S_OK;
}

vector<_uint> CPhysics_Terrain::Create_Indices(const _uint iNumUnitVertices)
{
	vector<_uint> TerrainIndices;
	TerrainIndices.reserve(iNumUnitVertices * iNumUnitVertices * 6);
	for (_uint i = 0; i < iNumUnitVertices; ++i)
	{
		for (_uint j = 0; j < iNumUnitVertices; ++j)
		{
			_uint iVertexIndex = i * (iNumUnitVertices + 1) + j;

			_uint RectIndex[4] =
			{
				iVertexIndex + (iNumUnitVertices + 1),
				iVertexIndex + (iNumUnitVertices + 1) + 1,
				iVertexIndex + 1,
				iVertexIndex,
			};

			TerrainIndices.push_back(RectIndex[0]);
			TerrainIndices.push_back(RectIndex[1]);
			TerrainIndices.push_back(RectIndex[2]);

			TerrainIndices.push_back(RectIndex[0]);
			TerrainIndices.push_back(RectIndex[2]);
			TerrainIndices.push_back(RectIndex[3]);
		}
	}

	return TerrainIndices;
}

CPhysics_Terrain* CPhysics_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysics_Terrain* pInstance = new CPhysics_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPhysics_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPhysics_Terrain* CPhysics_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFilePath, _uint iLevelID, const _wstring& strTerrainTag)
{
	CPhysics_Terrain* pInstance = new CPhysics_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strFilePath, iLevelID, strTerrainTag)))
	{
		MSG_BOX("Failed To Create : CPhysics_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPhysics_Terrain::Clone(void* pArg)
{
	CPhysics_Terrain* pInstance = new CPhysics_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Copy : CPhysics_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_Terrain::Free()
{
	super::Free();

	for (auto& pPhysics : m_SplitTerrainPhysics)
		Safe_Release(pPhysics);
	m_SplitTerrainPhysics.clear();

	//Safe_Release(m_pSplitTerrainPhysicsPrototype);
	Safe_Release(m_pPrototyeTags);
	Safe_Release(m_pLevelID);
}
