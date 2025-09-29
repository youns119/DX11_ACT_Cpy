#include "Physics_Convex.h"

#include "GameInstance.h"
#include "Physics_Function.h"

CPhysics_Convex::CPhysics_Convex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPhysics_Convex::CPhysics_Convex(const CPhysics_Convex& Prototype)
	: super(Prototype)
	, m_pCookingData { Prototype.m_pCookingData }
{
	Safe_AddRef(m_pCookingData);
}

HRESULT CPhysics_Convex::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysics_Convex::Initialize_Prototype(const _wstring& strFilePath, _uint iLevelID, const _wstring& strModelTag)
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
			MSG_BOX("Can Not Open File : Physics_Convex");
			return E_FAIL;
		}

		if (FAILED(Write_ConvexMesh_To_File(hFile, pModel)))
			return E_FAIL;

		Safe_Release(pModel);
	}

	/* 파일로부터 convex mesh 생성을 위한 정보 읽기 */
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Can Not Open File : Physics_Convex");
		_CrtDbgBreak();
		return E_FAIL;
	}
	if (FAILED(Read_ConvexMesh_From_File(hFile)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Convex::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	m_pFinalMatrix = pDesc->PhysicsDesc.pFinalMatrix;

	const _mat& WorldMatrix = pDesc->WorldMatrix;
	_vec3 vScale;
	_vec4 vQuat;
	_vec3 vPos;
	if (false == WorldMatrix.Decompose(vScale, vQuat, vPos))
		return E_FAIL;

	PxMeshScale pxScale = Convert(vScale);
	PxVec3 pxPos = Convert(vPos);
	PxQuat pxQuat = Convert(vQuat);
	PxTransform pxTransform(pxPos, pxQuat);
	PxMaterial* pMaterial = m_Physics.createMaterial(pDesc->PhysicsDesc.fStaticFriction, pDesc->PhysicsDesc.fDynamicFriction, pDesc->PhysicsDesc.fRestitution);

	if (true == pDesc->PhysicsDesc.isStatic)
	{
		m_pActor = m_Physics.createRigidStatic(pxTransform);
	}
	else
	{
		PxRigidDynamic* pRigidDynamic = m_Physics.createRigidDynamic(pxTransform);
		pRigidDynamic->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
		pRigidDynamic->setSleepThreshold(2.f);

		m_pActor = pRigidDynamic;
	}

	_vec3 vMin { 999.f, 999.f, 999.f }; // physics world max;
	_vec3 vMax { -999.f, -999.f, -999.f }; // physics world min;

	for (auto& CookData : m_pCookingData->Value())
	{
		PxU8* pData = reinterpret_cast<PxU8*>(CookData.data());
		PxU32 iLength = static_cast<PxU32>(CookData.size());

		PxDefaultMemoryInputData ReadBuffer(pData, iLength);
		PxConvexMesh* pConvexMesh = m_Physics.createConvexMesh(ReadBuffer);

		PxConvexMeshGeometry ConvexMeshGeom = PxConvexMeshGeometry(pConvexMesh, pxScale);
		//PxRigidActorExt::createExclusiveShape(*m_pActor, MeshGeom, *pMaterial);
		PxShape* pShape = m_Physics.createShape(ConvexMeshGeom, *pMaterial);

		if (nullptr == pShape)
			return E_FAIL;

		PxFilterData Filter;
		Filter.word0 = pDesc->PhysicsDesc.iFilter;
		pShape->setQueryFilterData(Filter);
		if (true == pDesc->isInChunk)
		{
			//pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			//pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			pShape->setFlag(PxShapeFlag::eVISUALIZATION, false);
		}

		m_pActor->attachShape(*pShape);

		PxBounds3 OutBound;
		PxGeometryQuery::computeGeomBounds(OutBound, ConvexMeshGeom, PxTransform(0.f, 0.f, 0.f));

		vMin = _vec3::Min(vMin, Convert(OutBound.minimum));
		vMax = _vec3::Max(vMax, Convert(OutBound.maximum));

		pShape->release();
	}

	if (FAILED(Setup_Mass(pDesc->PhysicsDesc, (vMin + vMax) * 0.5f)))
		return E_FAIL;

	if (true == pDesc->isInChunk)
		Resist_To_Chunk(vMin, vMax);
	
	m_pScene->addActor(*m_pActor);

	if (false == pDesc->PhysicsDesc.isStatic)
	{
		m_pGameInstance->Add_MovablePhysics_To_Chunk(vPos, m_iChunkIndexX, m_iChunkIndexZ);
		m_pGameInstance->Resist_DynamicPhysicsHash(m_pActor, this);
	}

	return S_OK;
}

HRESULT CPhysics_Convex::Render()
{
#ifdef _DEBUG
	//if (static_cast<PxRigidDynamic*>(m_pActor)->isSleeping())
	//{
	//	m_pGameInstance->Add_DebugRender_Function([]()->HRESULT
	//		{
	//			return CGameInstance::GetInstance()->Render_Font(L"Font_LiberationSans_20", L"Sleeping", { 640.f, 320.f });
	//		});
	//}
#endif // _DEBUG
	return S_OK;
}

HRESULT CPhysics_Convex::Write_ConvexMesh_To_File(HANDLE hFile, CModel* pModel)
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


		PxConvexMeshDesc ConvexDesc {};

		//ConvexDesc.setToDefault();
		ConvexDesc.points.count = static_cast<_uint>(pxPositions.size());
		ConvexDesc.points.stride = sizeof(PxVec3);
		ConvexDesc.points.data = pxPositions.data();

		ConvexDesc.vertexLimit = 255;
		//ConvexDesc.quantizedCount = 63;
		ConvexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxTolerancesScale ToleranceScale;
		PxCookingParams CookingParams(ToleranceScale);

		PxDefaultMemoryOutputStream WriteBuffer;
		PxConvexMeshCookingResult::Enum eResult;

		if (false == PxCookConvexMesh(CookingParams, ConvexDesc, WriteBuffer, &eResult))
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

HRESULT CPhysics_Convex::Read_ConvexMesh_From_File(HANDLE hFile)
{
	_ulong dwByte = 0;
	BOOL bResult = true;
	_uint iNumShape = 0;
	if (false == ReadFile(hFile, &iNumShape, sizeof(_uint), &dwByte, nullptr))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

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

CPhysics_Convex* CPhysics_Convex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysics_Convex* pInstance = new CPhysics_Convex(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CPhysics_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPhysics_Convex* CPhysics_Convex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFilePath, _uint iLevelID, const _wstring& strModelTag)
{
	CPhysics_Convex* pInstance = new CPhysics_Convex(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strFilePath, iLevelID, strModelTag)))
	{
		MSG_BOX("Failed To Create : CPhysics_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CPhysics_Convex::Clone(void* pArg)
{
	CPhysics_Convex* pInstance = new CPhysics_Convex(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CPhysics_Convex");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_Convex::Free()
{
	super::Free();

	Safe_Release(m_pCookingData);
}
