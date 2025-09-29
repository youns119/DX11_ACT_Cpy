#include "pch.h"
#include "MapLoader.h"

#include "GameInstance.h"

/* for.Level Static*/
#include "Camera_Free.h"

/* for.Level Logo*/
#include "BackGround.h"

/* for.Level GamePlay*/
#include "Terrain.h"
#include "Sky.h"
#include "ShellHouse.h"
#include "GrappleNeedle.h"
#include "HeartKelp.h"
#include "Urchin.h"
#include "PinBallBarge.h"
#include "PinBall_Bumper.h"
#include "PinBall_Spliter.h"
#include "PinBall_Spliter_L.h"
#include "PinBall_Flip.h"
#include "PinBall_Flip_L.h"
#include "Instancing_SeaWeeds.h"
#include "Instancing_KelpBig.h"
#include "Instancing_SeaGrass.h"
#include "Instancing_KelpGround.h"
#include "MapObject.h"
#include "MapTool_NPC.h"
#include "MapTrigger.h"
#include "MapTool_Item.h"
#include "MapTool_Monster.h"
#include "NonMeshCollider.h"
#include "RotatePipe.h"
#include "RotatePipeFloor.h"
#include "StreamVent.h"
#include "Sponge.h"  
#include "Spong.h"  
#include "Magnet.h"  
#include "Seagull.h"  
#include "Radio.h"  
#include "FakeMoonBrick.h"  
#include "Laser.h"

#include "Debug_PostProcess.h"

CMapLoader::CMapLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

//_uint APIENTRY Thread_Main(void* pArg)
_uint WINAPI Thread_Main(void* pArg)
{
	CMapLoader* pLoader = static_cast<CMapLoader*>(pArg);

	if (FAILED(pLoader->Start_Loading()))
	{
		MSG_BOX("Failed to Loading");
	}
	return 0;
}

HRESULT CMapLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);
	// m_LEVELID::LEVEL_STATIC가 필요로하는 자원을 로드해주기위한 스레드를 만들어낸다. 
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CMapLoader::Start_Loading()
{
	HRESULT hr{};

	// chat gpt 가 CoInitializeEx 코드는 임계영역 전에 작성하는게 좋다고 했음
	hr = CoInitializeEx(nullptr, 0);
	if (FAILED(hr)) // 내가 작성함
		return hr;

	EnterCriticalSection(&m_CriticalSection);

	switch (m_eNextLevelID)
	{
	case LEVELID::LEVEL_LOGO:
		hr = Loading_For_Level_Logo();
		break;

	case LEVELID::LEVEL_TOOL:
		hr = Loading_For_Level_Tool();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

_float CMapLoader::Get_CurrnetProgress() const
{
	_float fRatio = m_iCurrentLine / static_cast<_float>(m_iNumTotalLine);

	fRatio = min(1.f, fRatio);
	fRatio = max(0.f, fRatio);

	return fRatio;
}

#ifdef _DEBUG
void CMapLoader::SetUp_WindowText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}
#endif

HRESULT CMapLoader::Loading_For_Level_Logo()
{

#pragma region STATIC
	/* Load resoruce for Static */
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static"));

	/* for Static: Collider =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Collider"));
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider_AABB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider_OBB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for Static: Shader =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Shader"));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh_Instancing */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Instancing"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh_Instancing.hlsl"), VTXMESH_INSTANCE::Elements, VTXMESH_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	///* for Static: Navigation =================================================================== */
	//lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Navigation"));
	///* For.Prototype_Component_Navigation */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;


	/* for Static: Texture =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Texture"));
	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_Sky",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/SkyBox/Sky_%d.dds", 6))))
		return E_FAIL;

	/* for Static: VIBuffer =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: VIBuffer"));
	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for Static: NPC Model =================================================================== */

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: NPC Model"));
	if (FAILED(Loading_For_NPC()))
		return E_FAIL;

	/* for Static: Model =================================================================== */

	_matrix PreTransformMatrix = XMMatrixIdentity();
	lstrcpy(m_szLoadingText, TEXT("모델 원형을 생성한다."));

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static"));

	/* For.Prototype_Component_Model_Player*/
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Player.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sponge */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Sponge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Sponge.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_GrappleNeedle*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_GrappleNeedle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/GrappleNeedle.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_MoonShell */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_MoonShell"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/MoonShell.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_NC_Building_Gin_01 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NC_Building_Gin_01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/NC_Building_Gin_01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_NC_Building_Whiskey_01 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NC_Building_Whiskey_01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/NC_Building_Whiskey_01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cylinder00 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Cylinder.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cylinder01 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Cylinder.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cylinder02 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Cylinder.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cylinder03 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Cylinder.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: CoralTree"));
	/* For.Prototype_Component_Model_CoralTree_00 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CoralTree_00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CoralTree_00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CoralTree_01 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CoralTree_01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CoralTree_01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CoralTree_01 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CoralTree_02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CoralTree_02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CoralTree_03 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CoralTree_03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CoralTree_03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoralTree_04 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CoralTree_04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CoralTree_04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoralTree_05 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CoralTree_05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CoralTree_05.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoralBranch */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CoralBranch"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CoralBranch.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TubeCoral00 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TubeCoral00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TubeCoral00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TubeCoral01 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TubeCoral01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TubeCoral01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ShelfCoral_01 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ShelfCoral_01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ShelfCoral_01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ShelfCoral_02 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ShelfCoral_02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ShelfCoral_02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ShelfCoral_03 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ShelfCoral_03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ShelfCoral_03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ShelfCoral_04 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ShelfCoral_04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ShelfCoral_04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ShelfCoral_05 */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ShelfCoral_05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ShelfCoral_05.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Large_Plateau */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Large_Plateau"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Large_Plateau00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Spire"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Spire.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cluster_Large00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cluster_Large00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Cluster_Large00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cluster_Large01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cluster_Large01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Cluster_Large01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Large_Sharp_Cliff1*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Large_Sharp_Cliff"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Large_Sharp_Cliff.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashPile00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TrashPile00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/TrashPile00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashPile01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TrashPile01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/TrashPile01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashPile02*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TrashPile02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/TrashPile02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashPile04 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TrashPile04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/TrashPile04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClusterMedium*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ClusterMedium"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ClusterMedium.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClusterSmall*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ClusterSmall"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ClusterSmall.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PlatformyRock00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototyped_Component_Model_PlatformyRock00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PlatformyRock00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PlatformyRock01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_PlatformyRock01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PlatformyRock01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_HeartKelp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartKelp"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/HeartKelp.fbx", CModel::TYPE::PRE_TRANSFORM, XMMatrixIdentity()))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Urchin */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Urchin"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Urchin.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
#pragma endregion
#pragma region STATIC INTERACTION GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: GameObject"));

	/*For. Prototype_GameObject_Debug_PostProcess*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Debug_PostProcess"),
		CDebug_PostProcess::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_HeartKelp*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_HeartKelp"),
		CHeartKelp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Sponge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Sponge"),
		CSponge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Spong */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Spong"),
		CSpong::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Sky*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Camera_Free*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_MapTrigger */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_MapTrigger"),
		CMapTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_NonMeshCollider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_NonMeshCollider"),
		CNonMeshCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_MapTool_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_MapTool_Item"),
		CMapTool_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_MapTool_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_MapTool_Monster"),
		CMapTool_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_MapTool_NPC*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_MapTool_NPC"),
		CMapTool_NPC::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_MapObject*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_ShellHouse*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_ShellHouse"),
		CShellHouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_GrappleNeedle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_GrappleNeedle"),
		CGrappleNeedle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion 

#pragma region TERRAIN
	//맵별로 다른 텍스쳐 로드 해야댐..
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp"))))) // Field
		//CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/NoneHeight.bmp"))))) // CrabTomb, Beach
		return E_FAIL;
	/*For. Prototype_GameObject_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

//#pragma region BEACH
//
//	/* For.Prototype_Component_Model_Fork*/
//	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Fork"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Fork.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_Castle00 */
//	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Castle00"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Castle00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_Castle01 */
//	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Castle01"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Castle01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For.Prototype_Component_Model_Chips */
//	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Chips"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Chips.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For.Prototype_Component_Model_StarFish */
//	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_StarFish"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/StarFish.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For.Prototype_Component_Model_Umbrella */
//	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Umbrella"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Umbrella.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For.Prototype_Component_Model_Seagull */
//	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Seagull"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Seagull.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//#pragma endregion 
//#pragma region BEACH INTERACTION GAMEOBJECT
//	/*For. Prototype_GameObject_Seagull*/
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Seagull"),
//		CSeagull::Create(m_pDevice, m_pContext))))
//		return E_FAIL;
//#pragma endregion 

#pragma region VILLAGE
	lstrcpy(m_szLoadingText, TEXT("Loading Resource for Village"));

	/* For.Prototype_Component_Model_AlchoholBuilding*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_AlchoholBuilding"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/AlchoholBuilding.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Plane004 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Plane004"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Plane004.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Plane005 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Plane005"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Plane005.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Plane006 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Plane006"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Plane006.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_NC_Building_ReceiptRoad */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ReceiptRoad"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Receiptroad.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Village : TopSide"));
	/* For.Prototype_Component_Model_TopSide00 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TopSide00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide01 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TopSide01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide02 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TopSide02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide03 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TopSide03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide04 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TopSide04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Bridge */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Bridge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Bridge.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Village: JumpMap"));
	/* For.Prototype_Component_Model_StreetSign00 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_StreetSign00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/StreetSign00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_StreetSign01 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_StreetSign01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/StreetSign01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pole */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Pole"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Pole.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign0 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Sign0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign0.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign1 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Sign1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign1.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign2 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Sign2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign2.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign3 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Sign3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign3.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign4 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Sign4"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign4.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign5 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Sign5"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign5.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Spong */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Spong"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Spong.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
#pragma endregion 

#pragma region FIELD

	{ // Instancing
		lstrcpy(m_szLoadingText, TEXT("인스턴싱 로드 중"));

		/* For.Prototype_Component_Model_KelpBig */
		PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_KelpBig"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/KelpBig.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
			return E_FAIL;
		/* For.Prototype_Component_Model_SeaGrass00 */
		PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SeaGrass00"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SeaGrass00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
			return E_FAIL;
		/* For.Prototype_Component_Model_KelpGround */
		PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_KelpGround"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/KelpGround.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
			return E_FAIL;
		/* For.Prototype_Component_Model_KelpBushelTree*/
		PreTransformMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_KelpBushelTree"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/KelpBushelTree.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;
	}
	{
		/* For.Prototype_Component_VIBuffer_Instancing_Mesh_SeaGrass00 */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Mesh_SeaGrass00"),
			CVIBuffer_Instancing_Mesh::Create(m_pDevice, m_pContext, nullptr, TEXT("Prototype_Component_Model_SeaGrass00"), LEVEL_STATIC, L"../Bin/DataFiles/Map_InstancingMesh/SeaGrass00.instancing"))))
			return E_FAIL;
		/* For.Prototype_Component_VIBuffer_Instancing_Mesh_KelpGround */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Mesh_KelpGround"),
			CVIBuffer_Instancing_Mesh::Create(m_pDevice, m_pContext, nullptr, TEXT("Prototype_Component_Model_KelpGround"), LEVEL_STATIC, L"../Bin/DataFiles/Map_InstancingMesh/KelpGround.instancing"))))
			return E_FAIL;
		/* For.Prototype_Component_VIBuffer_Instancing_Mesh_KelpBushelTree */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Mesh_KelpBushelTree"),
			CVIBuffer_Instancing_Mesh::Create(m_pDevice, m_pContext, nullptr, TEXT("Prototype_Component_Model_KelpBushelTree"), LEVEL_STATIC, L"../Bin/DataFiles/Map_InstancingMesh/SeaWeeds.instancing"))))
			return E_FAIL;
		/* For.Prototype_Component_VIBuffer_Instancing_Mesh_Kelp */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Mesh_KelpBig"),
			CVIBuffer_Instancing_Mesh::Create(m_pDevice, m_pContext, nullptr, TEXT("Prototype_Component_Model_KelpBig"), LEVEL_STATIC, L"../Bin/DataFiles/Map_InstancingMesh/BigKelp.instancing"))))
			return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Field : JumpMap"));
	/* For.Prototype_Component_Model_Crane */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Crane"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Crane.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rail_00 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Rail_00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Rail_00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rail_01 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Rail_01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Rail_01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rail_02 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Rail_02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Rail_02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_RailSupport */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_RailSupport"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/RailSupportStrut.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Magnet */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Magnet"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Magnet.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_VentStair */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_VentStair"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/VentStair.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CrabDecal */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CrabDecal"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/CrabDecal.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_WoodPlank */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_WoodPlank"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/WoodPlank.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PinBallShip */
	PreTransformMatrix = XMMatrixScaling(0.0005f, 0.0005f, 0.0005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_PinBallShip"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PinBallShip.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Net */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Net"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Net.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SinkerHook */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SinkerHook"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SinkerHook.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ChainFloor2x2 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainFloor2x2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ChainFloor2x2.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Floor2x2 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Floor2x2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Floor2x2.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SquareFloor */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SquareFloor"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SquareFloor.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ChainFloor1x1 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ChainFloor1x1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ChainFloor1x1.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FloorChain */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FloorChain"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FloorChain.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FloorBolt */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FloorBolt"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FloorBolt.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_StreamVent */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_StreamVent"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/StreamVent.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PipeCurve */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_PipeCurve"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PipeCurve.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_JointPipe */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_JointPipe"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/JointPipe.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_T */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Pipe_T"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_T.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_Basic */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Pipe_Basic"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_Basic.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_Joint */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Pipe_Joint"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_Joint.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_Holder */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Pipe_Holder"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_Holder.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_Plate */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Pipe_Plate"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_Plate.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PropaneTank */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ProtpaneTank"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ProtpaneTank.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PropaneTank01 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ProtpaneTank01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ProtpaneTank01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_T_Rotate */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Pipe_T_Rotate"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_T_Rotate.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_T_RotateFloor */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Pipe_T_RotateFloor"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_T_RotateFloor.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Field : Environment"));

	/* For.Prototype_Component_Model_WoodPlate */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_WoodPlate"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/WoodPlate.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_MetalChair */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_MetalChair"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/MetalChair.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ButterTubTop */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ButterTubTop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ButterTubTop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ButterTub */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ButterTub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ButterTub.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashCan */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TrashCan"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TrashCan.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashCanTop */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TrashCanTop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TrashCanTop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_RustSeaChain*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_RustSeaChain"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/RustSeaChain.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SeaChain*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SeaChain"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/SeaChain.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Dumpster*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Dumpster"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Dumpster.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Anchor*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Anchor"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Anchor.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_LargeKelpTree*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_LargeKelpTree"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/LargeKelpTree.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_DuneFence00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_DuneFence00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/DuneFence00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_LifeRing*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_LifeRing"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/LifeRing.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Tank*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Tank"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Tank.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Tire*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Tire"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Tire.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FacilitiesContainer00 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FacilitiesContainer00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FacilitiesContainer00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FacilitiesContainer01 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FacilitiesContainer01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FacilitiesContainer01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cooler*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cooler"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Cooler.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoolerTop*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CoolerTop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/CoolerTop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_HoleTank*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_HoleTank"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/HoleTank.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Field : Azit"));

	/* For.Prototype_Component_Model_SharkObject */
	PreTransformMatrix = XMMatrixScaling(0.0000003f, 0.0000003f, 0.0000003f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SharkObject"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SharkObject.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Iron*/
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Iron"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Iron.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashPinClip*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_TrashPinClip"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TrashPinClip.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FeedersShop*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FeedersShop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/FeedersShop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BlackSmithShop*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_BlackSmithShop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/BlackSmithShop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PrawnShop*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_PrawnShop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/PrawnShop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CardBoardBuilding00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CardBoardBuilding00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CardBoardBuilding00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CardBoardBuilding01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CardBoardBuilding01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CardBoardBuilding01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CardBoardBuilding02*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CardBoardBuilding02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CardBoardBuilding02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CardBoardBuilding03*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_CardBoardBuilding03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CardBoardBuilding03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle00*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SandCastle00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SandCastle00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle01*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SandCastle01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SandCastle01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle02*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SandCastle02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SandCastle02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Flag*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Flag"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Flag.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

#pragma endregion


#pragma region FIELD INTERACTION GAMEOBJECT
	/*For. Prototype_GameObject_RotatePipe */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_RotatePipe"),
		CRotatePipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_RotatePipeFloor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_RotatePipeFloor"),
		CRotatePipeFloor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_StreamVent */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_StreamVent"),
		CStreamVent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Magnet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Magnet"),
		CMagnet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Urchin*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Urchin"),
		CUrchin::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/*For. Prototype_GameObject_Instancing_SeaWeeds*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_SeaWeeds"),
		CInstancing_SeaWeeds::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Instancing_KelpBig*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_KelpBig"),
		CInstancing_KelpBig::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Instancing_SeaGrass00*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_SeaGrass00"),
		CInstancing_SeaGrass::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Instancing_KelpGround*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Instancing_KelpGround"),
		CInstancing_KelpGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region PINBALL
	/* For.Prototype_Component_Model_PinBall */
	PreTransformMatrix = XMMatrixScaling(0.0005f, 0.0005f, 0.0005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_PinBallBarge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PinBall.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FlipL */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FlipL"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FlipperL.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FlipR */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FlipR"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FlipperR.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SpliterL */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SpliterL"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SpliterL.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SpliterR */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_SpliterR"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SpliterR.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BumperTop */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_BumperTop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/BumperTop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BumperBottom */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_BumperBottom"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/BumperBottom.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

#pragma endregion
#pragma region PINBALL INTERACTION GAMEOBJECT
	/*For. Prototype_GameObject_PinBallBarge*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_PinBallBarge"),
		CPinBallBarge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Bumper*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_PinBall_Bumper"),
		CPinBall_Bumper::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Spliter*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_PinBall_Spliter"),
		CPinBall_Spliter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Spliter*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_PinBall_Spliter_L"),
		CPinBall_Spliter_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Flip*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_PinBall_Flip"),
		CPinBall_Flip::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Flip_L*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_PinBall_Flip_L"),
		CPinBall_Flip_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region CLUB
	/* For.Prototype_Component_Model_GlowStickPurple */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_GlowStickPurple"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/GlowStickPurple.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Stage */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Stage"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Stage.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubCube*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ClubCube"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubCube.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubMesh*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ClubMesh"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubMesh.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubPipe00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ClubPipe00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubPipe00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubPipe01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ClubPipe01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubPipe01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubPipe02*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_ClubPipe02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubPipe02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Toilet*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Toilet"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Toilet.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Radio*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Radio"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Radio.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Tape*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Tape"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Tape.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	PreTransformMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Laser"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Laser.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
#pragma endregion 
#pragma region CLUB INTERACTION GAMEOBJECT
	/*For. Prototype_GameObject_Laser*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Laser"),
		CLaser::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Radio*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Radio"),
		CRadio::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion 
//
//#pragma region CRABTOMB
//	/* For.Prototype_Component_Model_FakeMoonBrick*/
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FakeMoonBrick"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/FakeMoonBrick.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_BleachedKingMolt_01 */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_BleachedKingMolt_01"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_BleachedKingMolt_02 */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_BleachedKingMolt_02"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_BleachedKingMolt_03 */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_BleachedKingMolt_03"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_BleachedKingMolt_04 */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_BleachedKingMolt_04"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_BleachedKingMolt_05 */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_BleachedKingMolt_05"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_05.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_MoonBrick */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_MoonBrickFloor"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/MoonBrick.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_MoonBrickBar */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_MoonBrickBar"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/MoonBrickBar.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_MoonBrickPilarBroken */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_MoonBrickPilarBroken"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/MoonBrickPilarBroken.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_MoonBrickPillar */
//	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_MoonBrickPilar"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/MoonBrickPilar.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//	/* For.Prototype_Component_Model_FeatherDuster */
//	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_FeatherDuster"),
//		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FeatherDuster.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
//		return E_FAIL;
//#pragma endregion

	/* 로고레벨을 위한 자원을 로드합니다.  */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 원형을 생성한다."));
	/* For.Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_LOGO, _T("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, _T("객체 원형을 생성"));

	/*For. Prototype_GameObject_BackGround*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_LOGO, _T("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_iCurrentLine = m_iNumTotalLine;

	lstrcpyW(m_szLoadingText, TEXT("로딩끝."));
	m_bIsFinished = true;

	return S_OK;
}



HRESULT CMapLoader::Loading_For_Level_Tool()
{
#pragma region TEXTURE
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 원형을 생성한다."));
	///* For.Prototype_Component_Texture_BackGround */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
	//	return E_FAIL;

	/* For.Prototype_Component_Texture_BackGround */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/SandBaseColor.png")))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/SandBaseColor%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Normal"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/SandNoiseNormalMap.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Normal_Sand"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/Sand_normal.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Normal_Wavy"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/SandWavey.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Field00_Mask2.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Brush"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png")))))
	//	return E_FAIL;

#pragma endregion
	 
	 
#pragma region NAVIGATION
	lstrcpyW(m_szLoadingText, TEXT("네비게이션 원형을 생성한다."));
#pragma endregion

#pragma region  SHADER
#pragma endregion


#pragma region MODEL
	

#pragma endregion
//

#pragma region GAME_OBJECT
	lstrcpy(m_szLoadingText, _T("객체 원형을 생성"));

	/*For. Prototype_GameObject_Dummy_NonAnim*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_FakeMoonBrick"),
		CFakeMoonBrick::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///*For. Prototype_GameObject_Dummy_Anim */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Dummy_Anim"),
	//	CDummy_Anim::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
#pragma endregion

	/* for Static: Navigation =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Navigation"));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 로딩이 완료되었습ㄴ미다 */
	lstrcpyW(m_szLoadingText, TEXT("로딩끝."));
	m_bIsFinished = true;

	return S_OK;
}

HRESULT CMapLoader::Loading_For_NPC()
{
	_matrix PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_NPC_BlackSmith */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_BlackSmith"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/BlackSmith/BlackSmith.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Chitan */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Chitan"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Chitan/Chitan.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Nemma */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Nemma"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Nemma/NPC_Nemma.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Pollip */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Pollip"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Pollip/Pollip.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Prawnathan */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Prawnathan"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Prawnathan/Prawnathan.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Sessi */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Sessi"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Sessi/Sessi.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Shark */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Shark"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Shark/Shark.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Topoda */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Topoda"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Topoda/Topoda.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Tortellini */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Tortellini"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Tortellini/Tortellini.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Urchin */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Urchin"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Urchin/Urchin.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_DanceCrab */
	PreTransformMatrix =  XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_DanceCrab"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/DanceCrab/DanceCrab.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Roland*/
	PreTransformMatrix = XMMatrixScaling(0.0045f, 0.0045f, 0.0045f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Roland"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Roland/Roland.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Inkerton*/
	PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Inkerton"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Inkerton/Inkerton.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Firth*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Firth"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Club_Firth/Player.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Chitan_Fight*/
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Chitan_Fight"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Chitan_Fight/Chitan_Fight.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Inkerton_Fight*/
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_NPC_Inkerton_Fight"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Inkerton_Fight/Inkerton_Fight.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homeless */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Homeless"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NPC/Homeless/Homeless.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

CMapLoader* CMapLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CMapLoader* pInstance = new CMapLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Create : CMapLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapLoader::Free()
{
	Engine::CBase::Free();

	WaitForSingleObject(m_hThread, INFINITE);
	//DeleteObject(m_hThread); // 이거 결국 사용 안하는듯, 리턴값도 false 임
	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
