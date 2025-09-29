#include "Physics_Mesh.h"

#include "GameInstance.h"
#include "Physics_Function.h"

CPhysics_Mesh::CPhysics_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:super(pDevice, pContext)
{
}

CPhysics_Mesh::CPhysics_Mesh(const CPhysics_Mesh& Prototype)
	: super(Prototype)
	, m_pCookingData { Prototype.m_pCookingData }
{
	Safe_AddRef(m_pCookingData);
}

HRESULT CPhysics_Mesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysics_Mesh::Initialize_Prototype(const _wstring& strFilePath, _uint iLevelID, const _wstring& strModelTag)
{
	m_pCookingData = CShared<vector<vector<_char>>>::Create();

	/* 파일이 없으면 prototype model 에서 즉시 만들고 파일로 저장 */
	if (false == CUtils::Is_FileExist(strFilePath))
	{
		CModel* pModel = dynamic_cast<CModel*>(m_pGameInstance->Clone_Prototype(iLevelID, strModelTag));
		if (nullptr == pModel)
			return E_FAIL;

		HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			MSG_BOX("Can Not Open File : Physics_Mesh");
			return E_FAIL;
		}

		if (FAILED(Write_TriangleMesh_To_File(hFile, pModel)))
			return E_FAIL;

		//Safe_Release(pModel);
	}

	/* 파일로부터 convex mesh 생성을 위한 정보 읽기 */
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Can Not Open File : Physics_Mesh");
		_CrtDbgBreak();
		return E_FAIL;
	}
	if (FAILED(Read_TriangleMesh_From_File(hFile)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Mesh::Initialize_Prototype(const _wstring& strFilePath, const vector<vector<_vec3>>& Vertices, const vector<vector<_uint>>& Indices)
{
	m_pCookingData = CShared<vector<vector<_char>>>::Create();

	/* 파일이 없으면 prototype model 에서 즉시 만들고 파일로 저장 */
	if (false == CUtils::Is_FileExist(strFilePath))
	{
		HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			MSG_BOX("Can Not Open File : Physics_Mesh");
			return E_FAIL;
		}

		if (FAILED(Write_TriangleMesh_To_File(hFile, Vertices, Indices)))
			return E_FAIL;
	}

	/* 파일로부터 convex mesh 생성을 위한 정보 읽기 */
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Can Not Open File : Physics_Mesh");
		_CrtDbgBreak();
		return E_FAIL;
	}
	if (FAILED(Read_TriangleMesh_From_File(hFile)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Mesh::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	if (false == pDesc->PhysicsDesc.isStatic)
		return E_FAIL;

	m_pFinalMatrix = pDesc->PhysicsDesc.pFinalMatrix;

	const _mat& WorldMatrix = pDesc->WorldMatrix;
	_vec3 vScale;
	_vec4 vQuat;
	_vec3 vPos;
	if (false == WorldMatrix.Decompose(vScale, vQuat, vPos))
		return E_FAIL;

	PxVec3 pxPos = Convert(vPos);
	PxQuat pxQuat = Convert(vQuat);
	PxTransform pxTransform(pxPos, pxQuat);
	PxMeshScale MeshScale { Convert(vScale) };

	PxMaterial* pMaterial = m_Physics.createMaterial(pDesc->PhysicsDesc.fStaticFriction, pDesc->PhysicsDesc.fDynamicFriction, pDesc->PhysicsDesc.fRestitution);

	/* rigid static 과 rigid dyamic (kinematic flag) 의 차이를 모르겠음 */
	m_pActor = m_Physics.createRigidStatic(pxTransform);

	_vec3 vMin { 999.f, 999.f, 999.f }; // physics world max;
	_vec3 vMax { -999.f, -999.f, -999.f }; // physics world min;

	for (auto& CookData : m_pCookingData->Value())
	{
		PxU8* pData = reinterpret_cast<PxU8*>(CookData.data());
		PxU32 iLength = static_cast<PxU32>(CookData.size());

		PxDefaultMemoryInputData eReadBuffer(pData, iLength);
		PxTriangleMesh* pCookedTerrainMesh = m_Physics.createTriangleMesh(eReadBuffer);

		PxTriangleMeshGeometry MeshGeom(pCookedTerrainMesh);
		MeshGeom.scale = MeshScale;

		PxShape* pShape = m_Physics.createShape(MeshGeom, *pMaterial);
		if (nullptr == pShape)
			return E_FAIL;

		//pShape->setContactOffset(0.1f);
		PxFilterData filter;
		filter.word0 = pDesc->PhysicsDesc.iFilter;
		pShape->setQueryFilterData(filter);
		if (true == pDesc->isInChunk)
		{
			//pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			//pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			pShape->setFlag(PxShapeFlag::eVISUALIZATION, false);
		}

		m_pActor->attachShape(*pShape);

		PxBounds3 OutBound;
		PxGeometryQuery::computeGeomBounds(OutBound, MeshGeom, pxTransform);

		vMin = _vec3::Min(vMin, Convert(OutBound.minimum));
		vMax = _vec3::Max(vMax, Convert(OutBound.maximum));

		pShape->release();
	}

	if (true == pDesc->isInChunk)
		Resist_To_Chunk(vMin, vMax);
	//else
		m_pScene->addActor(*m_pActor);

#ifdef _DEBUG
	m_vMin = vMin;
	m_vMax = vMax;
#endif // _DEBUG

	return S_OK;
}

HRESULT CPhysics_Mesh::Render()
{
#ifdef _DEBUG
	{
		_vec3 vCenter = _vec3((m_vMin + m_vMax) * 0.5f);
		_vec3 vScale = _vec3(m_vMax - m_vMin);

		_mat WorldMatrix {};
		WorldMatrix.right = _vec4(vScale.x, 0.f, 0.f, 0.f);
		WorldMatrix.up = _vec4(0.f, vScale.y, 0.f, 0.f);
		WorldMatrix.look = _vec4(0.f, 0.f, vScale.z, 0.f);
		WorldMatrix.position = vCenter;
		m_pGameInstance->Add_DebugRender_Box(WorldMatrix, _vec4(1.0f, 0.5f, 0.f, 1.f));
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CPhysics_Mesh::Write_TriangleMesh_To_File(HANDLE hFile, CModel* pModel)
{
	_ulong dwByte = 0;

	_uint iNumMeshes = static_cast<_uint>(pModel->Get_NumMeshes());
	WriteFile(hFile, &iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		const vector<VTXANIMMESH>& Vertices = pModel->Get_VertexData(i);
		vector<PxVec3> pxPositions;
		pxPositions.reserve(Vertices.size());
		for (auto& Vertex : Vertices)
			pxPositions.push_back(Convert(Vertex.vPosition));

		const vector<_uint>& Indices = pModel->Get_IndexData(i);

		PxTriangleMeshDesc MeshDesc;
		MeshDesc.points.count = static_cast<_uint>(pxPositions.size());
		MeshDesc.points.stride = sizeof(PxVec3);
		MeshDesc.points.data = pxPositions.data();

		MeshDesc.triangles.count = static_cast<_uint>(Indices.size() / 3);
		MeshDesc.triangles.stride = 3 * sizeof(_uint);
		MeshDesc.triangles.data = Indices.data();

		PxTolerancesScale ToleranceScale;
		PxCookingParams CookingParams(ToleranceScale);

		CookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eWELD_VERTICES;
		CookingParams.meshWeldTolerance = 0.01f;

		PxDefaultMemoryOutputStream WriteBuffer;
		PxTriangleMeshCookingResult::Enum eResult;
		if (false == PxCookTriangleMesh(CookingParams, MeshDesc, WriteBuffer, &eResult))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_uint iSize = WriteBuffer.getSize();
		WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, WriteBuffer.getData(), WriteBuffer.getSize(), &dwByte, nullptr);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CPhysics_Mesh::Write_TriangleMesh_To_File(HANDLE hFile, const vector<vector<_vec3>>& Meshes, const vector<vector<_uint>>& MeshIndices)
{
	_ulong dwByte = 0;

	_uint iNumMeshes = static_cast<_uint>(Meshes.size());
	WriteFile(hFile, &iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (_uint i=0; i<iNumMeshes; ++i)
	{
		const vector<_vec3>& Vertices = Meshes[i];
		vector<PxVec3> pxPositions;
		pxPositions.reserve(Vertices.size());
		for (auto& Vertex : Vertices)
			pxPositions.push_back(Convert(Vertex));

		const vector<_uint>& Indices = MeshIndices[i];

		PxTriangleMeshDesc MeshDesc;
		MeshDesc.points.count = static_cast<_uint>(pxPositions.size());
		MeshDesc.points.stride = sizeof(PxVec3);
		MeshDesc.points.data = pxPositions.data();

		MeshDesc.triangles.count = static_cast<_uint>(Indices.size() / 3);
		MeshDesc.triangles.stride = 3 * sizeof(_uint);
		MeshDesc.triangles.data = Indices.data();

		PxTolerancesScale ToleranceScale;
		PxCookingParams CookingParams(ToleranceScale);

		CookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eWELD_VERTICES;
		CookingParams.meshWeldTolerance = 0.01f;

		PxDefaultMemoryOutputStream WriteBuffer;
		PxTriangleMeshCookingResult::Enum eResult;
		if (false == PxCookTriangleMesh(CookingParams, MeshDesc, WriteBuffer, &eResult))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_uint iSize = WriteBuffer.getSize();
		WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, WriteBuffer.getData(), WriteBuffer.getSize(), &dwByte, nullptr);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CPhysics_Mesh::Read_TriangleMesh_From_File(HANDLE hFile)
{
	_ulong dwByte = 0;
	BOOL bResult = true;
	_uint iNumShape = 0;
	bResult &= ReadFile(hFile, &iNumShape, sizeof(_uint), &dwByte, nullptr);
	m_pCookingData->Value().reserve(iNumShape);

	for (_uint i = 0; i < iNumShape; ++i)
	{
		_uint iSize = 0;
		vector<_char> Data;
		bResult &= ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
		Data.resize(iSize);
		bResult &= ReadFile(hFile, Data.data(), iSize, &dwByte, nullptr);

		/* 실제 mesh -> geom -> shape 이 되는 과정은 initialize에서 */
		m_pCookingData->Value().push_back(Data);

		if (false == bResult)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
	}

	CloseHandle(hFile);
	return S_OK;
}

CPhysics_Mesh* CPhysics_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysics_Mesh* pInstance = new CPhysics_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPhysics_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPhysics_Mesh* CPhysics_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFilePath, _uint iLevelID, const _wstring& strModelTag)
{
	CPhysics_Mesh* pInstance = new CPhysics_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strFilePath, iLevelID, strModelTag)))
	{
		MSG_BOX("Failed To Create : CPhysics_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPhysics_Mesh* CPhysics_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFilePath, const vector<vector<_vec3>>& Vertices, const vector<vector<_uint>>& Indices)
{
	CPhysics_Mesh* pInstance = new CPhysics_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strFilePath, Vertices, Indices)))
	{
		MSG_BOX("Failed To Create : CPhysics_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPhysics_Mesh::Clone(void* pArg)
{
	CPhysics_Mesh* pInstance = new CPhysics_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Copy : CPhysics_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_Mesh::Free()
{
	super::Free();
	Safe_Release(m_pCookingData);
}
