#include "Physics_Manager.h"

#include "GameInstance.h"
#include "Physics_Chunk.h"
#include "Phsics_SimulationCallback.h"
#include "Physics_ControllerHitCallback.h"
#include "Physics_ControllerFilterCallback.h"
#include "Physics_Function.h"

HRESULT CPhysics_Manager::Initialize()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	m_pPvd = PxCreatePvd(*m_pFoundation);
	PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	m_pPvd->connect(*pTransport, PxPvdInstrumentationFlag::eALL);

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);

	return S_OK;
}

void CPhysics_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pScene)
		return;

	m_pScene->simulate(fTimeDelta);
	m_pScene->fetchResults(true);


	/* tood : 시뮬 이후의 피직스 컴포넌트의 위치를 기반으로 렌더링 전에 월드 위치를 동기화 해주어야함 */
	for (auto& pPhysics : m_PhysicsComs)
	{
		/* rigid static 인 경우라면 내부적으로 */
		pPhysics->Manipulate_Object();
		Safe_Release(pPhysics);
	}
	m_PhysicsComs.clear();
	for (auto& pController : m_ControllerComs)
	{
		pController->Manipulate_Object();
		Safe_Release(pController);
	}
	m_ControllerComs.clear();

#ifdef _DEBUG
	//CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//static _float fHeight = 15.f;
	//if (pGameInstance->Key_Press(KEY::UP))
	//	fHeight += 0.05f;
	//if (pGameInstance->Key_Press(KEY::DOWN))
	//	fHeight -= 0.05f;
	//_vec2 vWorldVolum = m_vWorldMax - m_vWorldMin;

	//_float fQuotienX;
	//_float fQuotienZ;

	//modf(vWorldVolum.x / m_fChunkSize, &fQuotienX);
	//modf(vWorldVolum.y / m_fChunkSize, &fQuotienZ);

	//_vec3 vOrigin { m_vWorldMin.x, fHeight, m_vWorldMin.y };
	//_vec3 vAxisX { fQuotienX * m_fChunkSize, 0.f, 0.f };
	//_vec3 vAxisY { 0.f, 0.f, fQuotienZ * m_fChunkSize };

	//pGameInstance->Add_DebugRender_Grid(
	//	vOrigin,
	//	vAxisX,
	//	vAxisY,
	//	m_fChunkSize, m_fChunkSize,
	//	_vec4(1.f, 0.f, 0.f, 1.f));
#endif // _DEBUG
}

HRESULT CPhysics_Manager::Ready_Scene()
{
	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_pCpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	SceneDesc.cpuDispatcher = m_pCpuDispatcher;
	SceneDesc.filterShader = PxDefaultSimulationFilterShader;

	m_pScene = m_pPhysics->createScene(SceneDesc);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);

	//m_pSimulationCallback = CPhsics_SimulationCallback::Create();
	//m_pScene->setSimulationEventCallback(m_pSimulationCallback);

	PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();
	if (pPvdClient)
	{
		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	return S_OK;
}

HRESULT CPhysics_Manager::Ready_ControllerManager()
{
	m_pControllerManager = PxCreateControllerManager(*m_pScene);
	m_pControllerManager->setOverlapRecoveryModule(true);
	//m_pControllerManager->setPreciseSweeps(true);

	m_pControllerHitCallback = CPhysics_ControllerHitCallback::Create(m_ActorToDynamicPhysicsCom);
	m_pControllerFilterCallback = CPhysics_ControllerFilterCallback::Create();

	return S_OK;
}

void CPhysics_Manager::Add_ChunkCount(_uint iX, _uint iZ)
{
	for (_int i = -1; i <= 1; ++i)
	{
		for (_int j = -1; j <= 1; ++j)
		{
			_int iAdjustedX = iX + i;
			_int iAdjustedZ = iZ + j;

			if (Out_Of_Boundary(iAdjustedX, iAdjustedZ))
				continue;

			m_Chunks[iAdjustedX][iAdjustedZ]->Add_Count();
		}
	}
}

void CPhysics_Manager::Sub_ChunkCount(_uint iX, _uint iZ)
{
	for (_int i = -1; i <= 1; ++i)
	{
		for (_int j = -1; j <= 1; ++j)
		{
			_int iAdjustedX = iX + i;
			_int iAdjustedZ = iZ + j;

			if (Out_Of_Boundary(iAdjustedX, iAdjustedZ))
				continue;

			m_Chunks[iAdjustedX][iAdjustedZ]->Sub_Count();
		}
	}
}

_bool CPhysics_Manager::Out_Of_Boundary(_int iX, _int iZ)
{
	return iX < 0 || iX >= m_Chunks.size() || iZ < 0 || iZ >= m_Chunks[0].size();
}

void CPhysics_Manager::Clear()
{
	for (auto& pPhysics : m_PhysicsComs)
		Safe_Release(pPhysics);
	m_PhysicsComs.clear();

	for (auto& pPhysics : m_ControllerComs)
		Safe_Release(pPhysics);
	m_ControllerComs.clear();

	for (auto& ChunkRow : m_Chunks)
		for (auto& pChunk : ChunkRow)
			Safe_Release(pChunk);
	m_Chunks.clear();

	//m_pControllerManager->purgeControllers(); 
	Safe_Release(m_pControllerHitCallback);
	Safe_Release(m_pControllerFilterCallback);
	PX_RELEASE(m_pControllerManager);

	Safe_Release(m_pSimulationCallback);
	PX_RELEASE(m_pScene);
	PX_RELEASE(m_pCpuDispatcher);

	m_ActorToDynamicPhysicsCom.clear();
}

void CPhysics_Manager::Simulate_Physics(CPhysics* pPhysics)
{
	m_PhysicsComs.push_back(pPhysics);
	Safe_AddRef(pPhysics);
}

void CPhysics_Manager::Simulate_Physics(CPhysics_Controller* pController)
{
	m_ControllerComs.push_back(pController);
	Safe_AddRef(pController);
}

PxController* CPhysics_Manager::Create_Controller(const CPhysics_Controller::DESC& Desc, CPhysics_Controller::USER_DATA* pUserData, _Out_ PxControllerFilters& outFilters, _Out_ _uint& iChunkIndexX, _Out_ _uint& iChunkIndexZ)
{
	if (nullptr == m_pControllerManager)
	{
		iChunkIndexX = 0;
		iChunkIndexZ = 0;
		outFilters = PxControllerFilters(nullptr, nullptr, m_pControllerFilterCallback);
		return nullptr;
	}


	PxCapsuleControllerDesc ControllerDesc = PxCapsuleControllerDesc();
	ControllerDesc.radius = Desc.fRadius;
	ControllerDesc.height = Desc.fHeight;
	ControllerDesc.climbingMode = Desc.eClimbingMode;

	ControllerDesc.position = ConvertExtended(Desc.vPosition);
	ControllerDesc.position.y += Desc.fRadius + Desc.fHeight * 0.5f;
	ControllerDesc.slopeLimit = Desc.fSlopeLimit;
	ControllerDesc.invisibleWallHeight = Desc.fInvisibleWallHeight;
	ControllerDesc.stepOffset = Desc.fStepOffset;
	ControllerDesc.density = Desc.fDensity;
	ControllerDesc.scaleCoeff = Desc.fScaleCoeff;
	ControllerDesc.nonWalkableMode = Desc.eNonWalkableMode;

	PxMaterial* pMaterial = PxGetPhysics().createMaterial(Desc.fStaticFriction, Desc.fDynamicFriction, Desc.fRestitution);
	ControllerDesc.material = pMaterial;

	ControllerDesc.reportCallback = m_pControllerHitCallback;

	ControllerDesc.userData = pUserData;

	//ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f);
	//ControllerDesc.maxJumpHeight = Desc.fMaxJumpHeight;
	//ControllerDesc.contactOffset = Desc.fContactOffset;
	//ControllerDesc.volumeGrowth = Desc.fVolumeGrowth;
	//ControllerDesc.registerDeletionListener = Desc.bRegisterDeletionListener;
	//ControllerDesc.clientID = Desc.ClientID;

	outFilters = PxControllerFilters(nullptr, nullptr, m_pControllerFilterCallback);

	Add_Movable_To_Chunk(Desc.vPosition, iChunkIndexX, iChunkIndexZ);

	return m_pControllerManager->createController(ControllerDesc);
}

void CPhysics_Manager::Add_Movable_To_Chunk(const _vec3& vPosition, _Out_ _uint& iChunkIndexX, _Out_ _uint& iChunkIndexZ)
{
	_vec2 vControllerPosition {};
	vControllerPosition.x = static_cast<_float>(vPosition.x);
	vControllerPosition.y = static_cast<_float>(vPosition.z);

	vControllerPosition = _vec2::Max(m_vWorldMin, _vec2::Min(m_vWorldMax, vControllerPosition));
	vControllerPosition -= m_vWorldMin;

	_uint iX = static_cast<_uint>(max(0, min(m_Chunks.size() - 1, static_cast<size_t>(vControllerPosition.x / m_fChunkSize))));
	_uint iZ = static_cast<_uint>(max(0, min(m_Chunks[0].size() - 1, static_cast<size_t>(vControllerPosition.y / m_fChunkSize))));

	Add_ChunkCount(iX, iZ);

	iChunkIndexX = iX;
	iChunkIndexZ = iZ;
}

void CPhysics_Manager::Resist_Physics_To_Chunk(_vec3 vMin, _vec3 vMax, CPhysics* pPhysics)
{
	/* 음수인 경우에 대해서도 처리 */
	vMin -= _vec3(m_vWorldMin.x, 0.f, m_vWorldMin.y);
	vMax -= _vec3(m_vWorldMin.x, 0.f, m_vWorldMin.y);
	//vMin = _vec3::Max(_vec3(), vMin); // max는 인트에서 알아서 잘린다.
	//vMin = _vec3::Max(_vec3( 이거 다시 봐야 함 ), vMin); // max는 인트에서 알아서 잘린다.

	_uint iMinX = max(0, static_cast<_int>(vMin.x / m_fChunkSize));
	_uint iMinZ = max(0, static_cast<_int>(vMin.z / m_fChunkSize));

	_uint iMaxX = max(0, static_cast<_int>(vMax.x / m_fChunkSize));
	_uint iMaxZ = max(0, static_cast<_int>(vMax.z / m_fChunkSize));

	/* 이미 chunk가 미리 잘 세팅 되었다고 가정한다. */
	iMaxX = min(iMaxX, static_cast<_uint>(m_Chunks.size()) - 1);
	iMinX = min(static_cast<_uint>(m_Chunks.size() - 1), max(iMinX, 0));
	iMaxZ = min(iMaxZ, static_cast<_uint>(m_Chunks[0].size()) - 1);
	iMinZ = min(static_cast<_uint>(m_Chunks[0].size() - 1), max(iMinZ, 0));

	for (size_t i = iMinX; i <= iMaxX; ++i)
	{
		for (size_t j = iMinZ; j <= iMaxZ; ++j)
		{
			m_Chunks[i][j]->Add_Physics(pPhysics);
		}
	}
}

// update chunk Index for movable physics : rename function 
void CPhysics_Manager::Update_ChunkIndex_On_Move(_vec3 vMovedPosition, _Inout_ _uint& iChunkIndexX, _Inout_ _uint& iChunkIndexZ)
{
	_vec2 vPhysicsWorldPosition = _vec2(vMovedPosition.x, vMovedPosition.z);
	vPhysicsWorldPosition = _vec2::Min(m_vWorldMax, _vec2::Max(m_vWorldMin, vPhysicsWorldPosition));
	vPhysicsWorldPosition -= m_vWorldMin;

	// 여기도 청크범위 넘어갔을때 처리 해주어야함
	//vMovedPosition -= _vec3(m_vWorldMin.x, 0.f, m_vWorldMin.y);
	//vMovedPosition = _vec3::Max(_vec3(), vMovedPosition); // max는 인트에서 알아서 잘린다.

	const _uint iNewX = max(0, min(static_cast<_uint>(m_Chunks.size() - 1), static_cast<_uint>(vPhysicsWorldPosition.x / m_fChunkSize)));
	const _uint iNewZ = max(0, min(static_cast<_uint>(m_Chunks[0].size() - 1), static_cast<_uint>(vPhysicsWorldPosition.y / m_fChunkSize)));

	if (false == (iNewX == iChunkIndexX && iNewZ == iChunkIndexZ))
	{
		Add_ChunkCount(iNewX, iNewZ);
		Sub_ChunkCount(iChunkIndexX, iChunkIndexZ);

		iChunkIndexX = iNewX;
		iChunkIndexZ = iNewZ;
	}
}

HRESULT CPhysics_Manager::SetUp_Scene(const _vec2& vMin, const _vec2& vMax, _float fChunkSize)
{
	if (FAILED(Ready_Scene()))
		return E_FAIL;

	if (FAILED(Ready_ControllerManager()))
		return E_FAIL;

	m_vWorldMin = vMin;
	m_vWorldMax = vMax;

	m_fChunkSize = fChunkSize;

	/* chunk */
	_vec2 vWorldVolume = m_vWorldMax - m_vWorldMin;
	_uint iChunkArrayWidth = static_cast<_uint>(vWorldVolume.x / m_fChunkSize) + 1;
	_uint iChunkArrayDepth = static_cast<_uint>(vWorldVolume.y / m_fChunkSize) + 1;
	m_Chunks.assign(iChunkArrayWidth, vector<CPhysics_Chunk*>(iChunkArrayDepth, nullptr));
	for (_uint i = 0; i < iChunkArrayWidth; ++i)
		for (_uint j = 0; j < iChunkArrayDepth; ++j)
			m_Chunks[i][j] = CPhysics_Chunk::Create();

	return S_OK;
}

void CPhysics_Manager::Remove_Controller_From_Chunk(_uint iX, _uint iZ)
{
	Sub_ChunkCount(iX, iZ);
}

_bool CPhysics_Manager::Ray_Cast(_vec3 vOrigin, _vec3 vDir, _float fLength, _Out_ _float& fDist, _uint iFilters) const
{
	vDir = vDir.Normalized();

	PxVec3 pxOrigin { vOrigin.x, vOrigin.y, vOrigin.z };
	PxVec3 pxDir { vDir.x, vDir.y, vDir.z };
	PxQueryFilterData filter; // query falg -> dynamic, static	
	filter.data.word0 = iFilters;
	
	PxRaycastBuffer hitBuffer;
	if (m_pScene->raycast(pxOrigin, pxDir, fLength, hitBuffer, PxHitFlag::eDEFAULT, filter))
	{
		fDist = hitBuffer.block.distance;
		return true;
	}

	fDist = fLength;
	return false;
}

void CPhysics_Manager::Erase_DynamicPhysicsHash(PxActor* pActor)
{
	auto iter = m_ActorToDynamicPhysicsCom.find(pActor);

	if (m_ActorToDynamicPhysicsCom.end() == iter)
		return;

	m_ActorToDynamicPhysicsCom.erase(iter);
}

void CPhysics_Manager::Resist_DynamicPhysicsHash(PxActor* pActor, CPhysics* pPhysics)
{
	auto iter = m_ActorToDynamicPhysicsCom.find(pActor);
	if (m_ActorToDynamicPhysicsCom.end() != iter)
	{
		MSG_BOX("유규빈에게 문의 주세요");
		_CrtDbgBreak();
	}
	m_ActorToDynamicPhysicsCom.emplace(pActor, pPhysics);
}

CPhysics_Manager* CPhysics_Manager::Create()
{
	CPhysics_Manager* pInstance = new CPhysics_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Create : CPhysics_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_Manager::Free()
{
	super::Free();

	for (auto& pPhysics : m_PhysicsComs)
		Safe_Release(pPhysics);
	m_PhysicsComs.clear();

	for (auto& pPhysics : m_ControllerComs)
		Safe_Release(pPhysics);
	m_ControllerComs.clear();

	for (auto& Row : m_Chunks)
	{
		for (auto& pChunk : Row)
			Safe_Release(pChunk);
	}
	m_Chunks.clear();

	Safe_Release(m_pControllerHitCallback);
	Safe_Release(m_pControllerFilterCallback);
	PX_RELEASE(m_pControllerManager);
	Safe_Release(m_pSimulationCallback);
	PX_RELEASE(m_pScene);
	PX_RELEASE(m_pCpuDispatcher);

	PX_RELEASE(m_pPhysics);

	if (m_pPvd)
	{
		PxPvdTransport* transport = m_pPvd->getTransport();
		m_pPvd->release();
		m_pPvd = nullptr;
		PX_RELEASE(transport);

	}
	PX_RELEASE(m_pFoundation);
}

