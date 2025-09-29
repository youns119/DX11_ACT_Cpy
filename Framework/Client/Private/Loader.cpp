#include "pch.h"
#include "Loader.h"

#include "GameInstance.h"

#include "UI_Headers.h"
#include "NPC_Headers.h"
#include "FileData_Locator.h"
#include "Service_Item.h"
#include "Service_SKill.h"
#include "Service_Shell.h"
#include "Service_Dialogue.h"
#include "Service_Sheleport.h"
#include "PlayerManager.h"
#include "PlayerInstance.h"

/* for.Level Static*/
#include "Camera_Free.h"
#include "CinematicCamera.h"
#include "Player.h"
#include "Weapon_Player.h"
#include "Hand_Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "CineCamera_Player.h"
#include "StateMachine_Player.h"
#include "StateMachine_PlayerUpper.h"
#include "Sardine_StateMachine.h"
#include "Normie_StateMachine.h"
#include "Gumpounder_StateMachine.h"
#include "Bowman_StateMachine.h"
#include "BobbitWorm_StateMachine.h"
#include "Splitter_StateMachine.h"
#include "Inkerton_StateMachine.h"
#include "Pagurus_StateMachine.h"
#include "Inkerton_EncounterStateMachine.h"
#include "Roland_StateMachine.h"
#include "Petroch_StateMachine.h"
#include "Cuttlefish_StateMachine.h"
#include "Frogfish_StateMachine.h"
#include "Enemy_Manager.h"
#include "Movement.h"
#include "CameraComponent.h"
#include "Item_Trash.h"
#include "Shell_World.h"
#include "Shell_Part.h"
#include "Item_HeartKelp.h"
#include "Item_BottleCap.h"
#include "ShellSkill_RollOut.h"
#include "ShellSkill_Fizzle.h"
#include "ActiveSkill_MantisPunch.h"
#include "ActiveSkill_Urchin.h"
#include "ActiveSkill_BobbitTrap.h"
#include "Skill_Topoda.h"
#include "Projectile_Manager.h"
#include "Projectile_Fizzle.h"
#include "Projectile_Grapple.h"
#include "Projectile_Urchin.h"
#include "Remains.h"
#include "MapTrigger.h"
#include "NonMeshCollider.h"
#include "TriggerObserverTest.h"
#include "CineCamera.h"
#include "WaterShadow.h"
#include "Theme_Manager.h"
#include "Visit_Manger.h"
#include "Light_Player.h"

/* for.Level Logo*/

/* for.Level Village*/
#include "Terrain.h"
#include "CrabTomb_Terrain.h"
#include "Beach_Terrain.h"
#include "Tutorial_Terrain.h"
#include "Sky.h"
#include "Dummy_NonAnim.h"
#include "Dummy_Anim.h"
#include "Enemy_Sardine.h"
#include "Enemy_Cuttlefish.h"
#include "Cuttlefish_Fist.h"
#include "Cuttlefish_Tail.h"
#include "Cuttlefish_GrabTentacle.h"
#include "Cuttlefish_Missile.h"
#include "Enemy_Frogfish.h"
#include "Boss_Roland.h"
#include "Sardine_Horn.h"
#include "Sardine_Tail.h"
#include "Enemy_Splitter.h"
#include "Splitter_Weapon.h"
#include "Splitter_LeftClaw.h"
#include "Enemy_Normie.h"
#include "Normie_LeftClaw.h"
#include "Enemy_Gumpounder.h"
#include "Gumpounder_Hammer.h"
#include "Enemy_Bowman.h"
#include "Bowman_RightClaw.h"
#include "Bowman_Arrow.h"
#include "Bowman_Band.h"
#include "Arrow.h"
#include "Enemy_BobbitWorm.h"
#include "BobbitWorm_Mouth.h"
#include "BobbitWorm_playerDetector.h"
#include "Boss_Pagurus.h"
#include "Pagurus_Fork.h"
#include "Boss_Petroch.h"
#include "Petroch_Shell.h"
#include "Petroch_Blockable.h"
#include "Petroch_SharpCrystal.h"
#include "Petroch_CrystalSpawner.h"
#include "Petroch_LeftClaw.h"
#include "Boss_Inkerton.h"
#include "Inkerton_Gun.h"
#include "Inkerton_Bullet.h"
#include "Inkerton_Nutcracker.h"
#include "Inkerton_StretchedNutcracker.h"
#include "Roland_Fist.h"
#include "Roland_Belly.h"
#include "Roland_Wrench_0_L.h"
#include "Roland_Wrench_1_L.h"
#include "Roland_Wrench_2_L.h"
#include "Roland_Wrench_0_R.h"
#include "Roland_Wrench_1_R.h"
#include "Roland_Wrench_2_R.h"

/*PinBall*/
#include "PinBallBarge.h"
#include "PinBall_Bumper.h"
#include "PinBall_Spliter.h"
#include "PinBall_Spliter_L.h"
#include "PinBall_Flip.h"
#include "PinBall_Flip_L.h"

/* Beach */
#include "WaterPlane.h"
#include "Seagull.h"  

/*CrabTomb*/
#include "FakeMoonBrick.h"

/*Club*/
#include "Radio.h"  
#include "Laser.h"

/*Field*/
#include "RotatePipe.h"
#include "RotatePipeFloor.h"
#include "StreamVent.h"
#include "Magnet.h"  
#include "NPC_Shark_Shell.h"

/*Map*/
#include "MapObject.h"
#include "Instancing_SeaWeeds.h"
#include "Instancing_KelpBig.h"
#include "Instancing_SeaGrass.h"
#include "Instancing_KelpGround.h"
#include "ShellHouse.h"
#include "GrappleNeedle.h"
#include "HeartKelp.h"
#include "Urchin.h"
#include "Sponge.h"  
#include "Spong.h"  


/* Effect */
#include "Effect_GodRay.h"
#include "Effect_Textured_Rope.h"
#include "Effect_Textured_Trail.h"
#include "Effect_Colored_Trail.h"

#include "Effect_Sprite_Once.h"
#include "Effect_Sprite_Loop.h"
#include "Effect_Sprite_Loop_NonBlend.h"
#include "Effect_Change_ExplToRiseW.h"

#include "Effect_Mesh_Once.h"
#include "Effect_Mesh_Increase.h"
#include "Effect_Mesh_ActivateShell.h"
#include "Effect_Mesh_ActivateShellWind.h"
#include "Effect_Player_AttNormal.h"
#include "Effect_Player_AttSwipe.h"
#include "Effect_Player_HammerOnce.h"
#include "Effect_RadialDistortion.h"
#include "Effect_Enemy_Fear.h"
#include "Effect_Splitter_Swipe.h"
#include "Effect_Roland_Increase.h"
#include "Effect_Roland_Slash.h"
#include "Effect_Roland_BigCombo.h"
#include "Effect_Roland_BigSwipe.h"
#include "Effect_Inkerton_Increase.h"
#include "Effect_Inkerton_ScalingY.h"

#include "Effect_MeshInst_Once.h"
#include "Effect_MeshInst_NonBlend.h"

#include "Fork.h"

// CameraTool
#include "CameraTool.h"
#include "CineCameraTool.h"
#include "CineActorTool.h"
#include "CineDebugTool.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

//_uint APIENTRY Thread_Main(void* pArg)
static _uint WINAPI Thread_Main(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Start_Loading()))
	{
		MSG_BOX("Failed to Loading");
	}

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);
	// m_eNextLevelID가 필요로하는 자원을 로드해주기위한 스레드를 만들어낸다. 
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start_Loading()
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

	case LEVELID::LEVEL_VILLAGE:
		hr = Loading_For_Level_Village();
		break;

	case LEVELID::LEVEL_FIELD:
		hr = Loading_For_Level_Field();
		break;

	case LEVELID::LEVEL_PINBALL:
		hr = Loading_For_Level_PinBall();
		break;

	case LEVELID::LEVEL_CRABTOMB:
		hr = Loading_For_Level_CrabTomb();
		break;

	case LEVELID::LEVEL_BEACH:
		hr = Loading_For_Level_Beach();
		break;

	case LEVELID::LEVEL_CLUB:
		hr = Loading_For_Level_Club();
		break;

	case LEVELID::LEVEL_TUTORIAL:
		hr = Loading_For_Level_Tutorial();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

_float CLoader::Get_CurrnetProgress() const
{
	_float fRatio = m_iCurrentLine / static_cast<_float>(m_iNumTotalLine);

	fRatio = min(1.f, fRatio);
	fRatio = max(0.f, fRatio);

	return fRatio;
}

#ifdef _DEBUG
void CLoader::SetUp_WindowText() const
{
	SetWindowText(g_hWnd, m_szLoadingText);
}
#endif

HRESULT CLoader::Loading_For_Level_Static()
{
	/* Load resoruce for Static */
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static"));

	if (FAILED(Loading_Effects(LEVEL_STATIC)))
		return E_FAIL;

	if (FAILED(Loading_FileData()))
		return E_FAIL;

	if (FAILED(Loading_Player(LEVELID::LEVEL_STATIC)) ||
		FAILED(Loading_Equipment(LEVELID::LEVEL_STATIC)) ||
		FAILED(Loading_Item(LEVELID::LEVEL_STATIC)) ||
		FAILED(Loading_Monster(LEVELID::LEVEL_STATIC)) ||
		FAILED(Loading_NPC(LEVELID::LEVEL_STATIC)) ||
		FAILED(Loading_Projectile(LEVELID::LEVEL_STATIC)))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : DummyObject"));

	if (FAILED(Loading_DummyObject(LEVEL_STATIC)))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : UITexture"));

	if (FAILED(Loading_Static_UI_Texture()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : UIGameObject"));

	if (FAILED(Loading_Static_UI_GameObject()))
		return E_FAIL;

#ifdef _DEBUG

	if (FAILED(Loading_CameraTool(LEVELID::LEVEL_STATIC)))
		return E_FAIL;

#endif

#pragma region STATIC_COLLIDER

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

#pragma endregion

#pragma region STATIC_PHYSICS

	/* for Static: Physics =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Physics"));
	/* For.Prototype_Component_Physics_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Physics_Sphere"),
		CPhysics_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Capsule */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Physics_Capsule"),
		CPhysics_Capsule::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Convex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Physics_Convex"),
		CPhysics_Convex::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Controller */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Physics_Controller"),
		CPhysics_Controller::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Physics_Mesh"),
		CPhysics_Mesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Physics_Terrain"),
		CPhysics_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region STATIC_SHADER

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

#pragma endregion

#pragma region STATIC_NAVIGATION

	/* for Static: Navigation =================================================================== */

#pragma endregion

#pragma region STATIC_MOVEMENT

	// Prototype_Component_Movement
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Movement"),
		CMovement::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region STATIC_CAMERACOMPONENT

	// Prototype_Component_Camera
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		CCameraComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region STATIC_TEXTURE

	/* for Static: Texture =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Texture"));

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_Sky",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/SkyBox/Sky.dds", 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Field : Texture"));

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Terrain"), 
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/SandBaseColor%d.png"), 3))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Normal */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Normal"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/SandNoiseNormalMap.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Normal_Sand */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Normal_Sand"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/Sand_normal.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Normal_Wavy */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, _T("Prototype_Component_Texture_Normal_Wavy"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/SandWavey.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Field00_Mask2.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_MS_Sand */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MS_Sand"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/SandMetalicSmoothness.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_EnemyDessolveNoise */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_EnemyDessolveNoise",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Enemy/Noise/Dissolve_Noise_Texture%d.png", 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WaterShadow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_WaterShadow",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/WaterCaustics/WaterShadow.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WaterCausticsDistortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_WaterCausticsDistortion",
		//CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Water_Caustics_Med.png"))))
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/WaterCaustics/waterDistortion.png"))))
		return E_FAIL;

#pragma endregion

#pragma region STATIC_VIBUFFER

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

#pragma endregion

#pragma region STATIC_MODEL

	/* for Static: Model =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Model"));

	_matrix PreTransformMatrix = XMMatrixIdentity();

	{
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
		/* For.Prototype_Component_Model_HeartKelp */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartKelp"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/HeartKelp.fbx", CModel::TYPE::PRE_TRANSFORM, XMMatrixIdentity()))))
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

	lstrcpy(m_szLoadingText, TEXT("맵 오브젝트 모델 로드 중"));

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Spire"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Spire.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_GrappleNeedle*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_GrappleNeedle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/GrappleNeedle.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sponge */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Sponge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Sponge.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Spong */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Spong"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Spong.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Large_Plateau */
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Large_Plateau"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Large_Plateau00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_PlatformyRock00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_PlatformyRock00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PlatformyRock00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_PlatformyRock01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_PlatformyRock01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PlatformyRock01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cluster_Large00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cluster_Large00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Cluster_Large00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cluster_Large01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Cluster_Large01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Cluster_Large01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
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

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Cylinder"));

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

	/* For.Prototype_Component_Model_GlowStickPurple */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_GlowStickPurple"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/GlowStickPurple.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_GlowStickBlue */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_GlowStickBlue"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/GlowStickBlue.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: Building"));
	/* For.Prototype_Component_Model_Bridge */
	//PreTransformMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Bridge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Bridge.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
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

	/* For.Prototype_Component_Model_MoonShell */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_MoonShell"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/MoonShell.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
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
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Plane006.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Urchin */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Model_Urchin"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Urchin.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region STATIC_PHYSICS

	/* For.Prototype_Component_Physics_Mesh_ClusterMedium */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_ClusterMedium",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/ClusterMedium.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_ClusterMedium"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_ClusterSmall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_ClusterSmall",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/ClusterSmall.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_ClusterSmall"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_PlatformyRock01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_PlatformyRock01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/PlatformyRock01.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_PlatformyRock01"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_TrashPile00 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_TrashPile00",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/TrashPile00.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_TrashPile00"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_TrashPile01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_TrashPile01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/TrashPile01.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_TrashPile01"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TrashPile02*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_TrashPile02",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/TrashPile02.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_TrashPile02"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_TrashPile04*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_TrashPile04",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/TrashPile04.physTriangle",
			LEVELID::LEVEL_STATIC, L"Prototype_Component_Model_TrashPile04"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_Large_Plateau */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_Large_Plateau",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/Large_Plateau.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_Large_Plateau"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_Bridge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_Bridge",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/Bridge.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_Bridge"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_NC_Building_Gin_01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_NC_Building_Gin_01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/NC_Building_Gin_01.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_NC_Building_Gin_01"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_NC_Building_Whiskey_01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_NC_Building_Whiskey_01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/NC_Building_Whiskey_01.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_NC_Building_Whiskey_01"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_MoonShell */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_MoonShell",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/MoonShell.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_MoonShell"))))
		return E_FAIL;

	/* For.Prototype_Component_PHysics_Mesh_Plane004 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_Plane004",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/Plane004.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_Plane004"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_Plane005 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_Plane005",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/Plane005.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_Plane005"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_Plane006 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_Plane006",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/Plane006.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_Plane006"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_ShelfCoral_01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_ShelfCoral_01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/ShelfCoral_01.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_ShelfCoral_01"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_ShelfCoral_02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_ShelfCoral_02",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/ShelfCoral_02.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_ShelfCoral_02"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_ShelfCoral_03 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_ShelfCoral_03",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/ShelfCoral_03.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_ShelfCoral_03"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_ShelfCoral_04 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_ShelfCoral_04",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/ShelfCoral_04.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_ShelfCoral_04"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_ShelfCoral_05 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Physics_Mesh_ShelfCoral_05",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Static/ShelfCoral_05.physTriangle",
			LEVEL_STATIC, L"Prototype_Component_Model_ShelfCoral_05"))))
		return E_FAIL;
#pragma endregion

#pragma region STATIC_GAMEOBJECT

	/* for Static: GameObject =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: GameObject"));

	/*For. Prototype_GameObject_Sky*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Camera_Free*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Item_Trash*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Item_Trash"),
		CItem_Trash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_CinematicCamera*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_CinematicCamera"),
		CCinematicCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Remains*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Remains"),
		CRemains::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_MapTrigger */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_MapTrigger"),
		CMapTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_TriggerObserverTest */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_TriggerObserverTest"),
		CTriggerObserverTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_CineCamera*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_CineCamera"),
		CCineCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_MapObject*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"), CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_ShellHouse*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_ShellHouse"),
		CShellHouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_GrappleNeedle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_GrappleNeedle"),
		CGrappleNeedle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Sponge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Sponge"),
		CSponge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Spong */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Spong"),
		CSpong::Create(m_pDevice, m_pContext))))
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
	/*For. Prototype_GameObject_HeartKelp*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_HeartKelp"),
		CHeartKelp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Urchin*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Urchin"),
		CUrchin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//================================임시 끝==========================================//

	/*For. Prototype_GameObject_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_WaterShadow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_WaterShadow"),
		CWaterShadow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_ThemeManager */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_ThemeManager",
		CTheme_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_VisitManager */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_VisitManager",
		CVisit_Manger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Light_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_GameObject_Light_Player",
		CLight_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Tutorial()
{

#pragma region TUTORIAL_MODEL

	_matrix PreTransformMatrix = XMMatrixIdentity();
	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Tutorial : Model"));

#pragma endregion


#pragma region TUTORIAL_VIBUFFER

	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Tutorial : VIBuffer"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Terrain_Tutorial"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/MakeTutorialMap.bmp"), false))))
		return E_FAIL;

#pragma endregion


#pragma region TUTORIAL_PHYSICS

	/* For.Prototype_Component_Physics_Terrain_Tutorial */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_TUTORIAL, L"Prototype_Component_Physics_Terrain_Tutorial",
		CPhysics_Terrain::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Tutorial/Tutorial_Terrain.physTriangle",
			LEVELID::LEVEL_TUTORIAL, L"Prototype_Component_VIBuffer_Terrain_Tutorial"))))
		return E_FAIL;

#pragma endregion

#pragma region TUTORIAL_GAMEOBJECT

	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Tutorial : GameObject"));
	/*For. Prototype_GameObject_Tutorial_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_TUTORIAL, TEXT("Prototype_GameObject_Tutorial_Terrain"),
		CTutorial_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpyW(m_szLoadingText, TEXT("로딩 끝"));
	m_bIsFinished = true;


	return S_OK;

}

HRESULT CLoader::Loading_For_Level_Logo()
{
	if (FAILED(Loading_For_Level_Static()))
		return E_FAIL;

	/* 로고레벨을 위한 자원을 로드합니다.  */
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Logo"));

#pragma region LOGO_TEXTURE

	/* for Logo: Texture =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Logo: Texture"));

	/* For.Prototype_Component_Texture_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_LOGO, TEXT("Prototype_Component_Texture_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Background.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo.dds"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region LOGO_PROTOTYPE

	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성"));

	/*For. Prototype_GameObject_UI_MainMenu*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_LOGO, TEXT("Prototype_GameObject_UI_MainMenu"), CUI_MainMenu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Logo*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_LOGO, TEXT("Prototype_GameObject_UI_Logo"), CUI_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpyW(m_szLoadingText, TEXT("로딩끝."));
	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Village()
{
	/* Load resoruce for Static */
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Village"));

#pragma region VILLAGE_TEXTURE

	lstrcpy(m_szLoadingText, TEXT("텍스쳐 원형을 생성한다."));
#pragma endregion

#pragma region VILLAGE_NAVIGATION

	lstrcpyW(m_szLoadingText, TEXT("네비게이션 원형을 생성한다."));

#pragma endregion

#pragma region VILLAGE_SHADER

	/* For.Prototype_Component_Shader_VtxPosTex_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Shader_VtxPosTex_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_Particle.hlsl"),
			VTXPOSTEX_PARTICLE::Elements, VTXPOSTEX_PARTICLE::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region VILLAGE_MODEL

	lstrcpy(m_szLoadingText, TEXT("모델 원형을 생성한다."));

	_matrix PreTransformMatrix = XMMatrixIdentity();

	{
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: TopSide"));

	/* For.Prototype_Component_Model_StreetSign00 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_StreetSign00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/StreetSign00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_StreetSign01 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_StreetSign01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/StreetSign01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pole */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Pole"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Pole.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign0 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign0.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign1 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign1.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign2 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign2.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign3 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign3.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign4 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign4"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign4.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Sign5 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign5"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Sign5.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_AlchoholBuilding*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_AlchoholBuilding"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/AlchoholBuilding.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide00 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TopSide00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide01 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TopSide01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide02 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TopSide02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide03 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TopSide03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TopSide04 */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_TopSide04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TopSide04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ReceiptRoad */
	PreTransformMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Model_ReceiptRoad"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Receiptroad.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Particle_Rect */
	{
		/* For.Snow */
		CVIBuffer_InstancingParticle::DESC Desc{};

		Desc.iNumInstance = 1000;
		Desc.vCenter = _float3(64.f, 20.f, 64.f);
		Desc.vRange = _float3(128.f, 1.f, 128.f);
		Desc.vSize = _float2(0.2f, 0.5f);
		Desc.vSpeed = _float2(2.f, 5.f);
		Desc.vLifeTime = _float2(3.0f, 5.5f);
		Desc.isLoop = true;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
			CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, Desc))))
			return E_FAIL;
	}

#pragma endregion

#pragma region VILLAGE_PHYSICS
	/* For.Prototype_Component_Physics_Mesh_Sign0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_Sign0",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/Sign0.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_Sign0"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_Sign1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_Sign1",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/Sign1.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_Sign1"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_Sign2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_Sign2",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/Sign2.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_Sign2"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_Sign3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_Sign3",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/Sign3.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_Sign3"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_Sign4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_Sign4",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/Sign4.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_Sign4"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_Sign5 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_Sign5",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/Sign5.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_Sign5"))))
		return E_FAIL;


	/* For.Prototype_Component_Physics_Mesh_TopSide00 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_TopSide00",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/TopSide00.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_TopSide00"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_TopSide01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_TopSide01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/TopSide01.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_TopSide01"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_TopSide02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_TopSide02",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/TopSide02.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_TopSide02"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_TopSide03 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_TopSide03",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/TopSide03.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_TopSide03"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_TopSide04 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_TopSide04",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/TopSide04.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_TopSide04"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_ReceiptRoad */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, L"Prototype_Component_Physics_Mesh_ReceiptRoad",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/GamePlay/ReceiptRoad.physTriangle",
			LEVEL_VILLAGE, L"Prototype_Component_Model_ReceiptRoad"))))
		return E_FAIL;

#pragma endregion


#pragma region VILLAGE_PROTOTYPE

	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성"));

	/*For. Prototype_GameObject_Sardine_Horn*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_GameObject_Sardine_Horn"),
	//	CSardine_Horn::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

#pragma endregion

	/* 로딩이 완료되었습ㄴ미다 */
	lstrcpyW(m_szLoadingText, TEXT("로딩끝."));
	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Field()
{
	/* 로딩이 완료되었습니다. */
	lstrcpyW(m_szLoadingText, TEXT("필드 로드 중."));

#pragma region FIELD_VIBUFFER

	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Field : VIBuffer"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/MakeHeightMap.bmp"), false))))
		return E_FAIL;

#pragma endregion


#pragma region FIELD_MODEL
	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Field : Model"));

	_matrix PreTransformMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_SharkObject */
	PreTransformMatrix = XMMatrixScaling(0.0000003f, 0.0000003f, 0.0000003f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_SharkObject"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SharkObject.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Iron*/
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Iron"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Iron.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashPinClip*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_TrashPinClip"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TrashPinClip.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FeedersShop*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_FeedersShop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/FeedersShop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BlackSmithShop*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_BlackSmithShop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/BlackSmithShop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PrawnShop*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_PrawnShop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/PrawnShop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CardBoardBuilding00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_CardBoardBuilding00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CardBoardBuilding00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CardBoardBuilding01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_CardBoardBuilding01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CardBoardBuilding01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CardBoardBuilding02*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_CardBoardBuilding02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CardBoardBuilding02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CardBoardBuilding03*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_CardBoardBuilding03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/CardBoardBuilding03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_WoodPlate */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_WoodPlate"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/WoodPlate.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_MetalChair */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_MetalChair"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/MetalChair.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ButterTubTop */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_ButterTubTop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ButterTubTop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ButterTub */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_ButterTub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/ButterTub.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashCan */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_TrashCan"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TrashCan.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_TrashCanTop */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_TrashCanTop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/TrashCanTop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_RustSeaChain*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_RustSeaChain"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/RustSeaChain.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SeaChain*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_SeaChain"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/SeaChain.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Dumpster*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Dumpster"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Dumpster.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Anchor*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Anchor"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/Anchor.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle00*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_SandCastle00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SandCastle00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle01*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_SandCastle01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SandCastle01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle02*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_SandCastle02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SandCastle02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Flag*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Flag"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Flag.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;



	/*PinBall 가는 길*/

	/* For.Prototype_Component_Model_PinBallShip */
	PreTransformMatrix = XMMatrixScaling(0.0005f, 0.0005f, 0.0005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_PinBallShip"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PinBallShip.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Net */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Net"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Net.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_RopeBake */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_RopeBake"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/RopeBake.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SinkerHook */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_SinkerHook"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SinkerHook.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ChainFloor2x2 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_ChainFloor2x2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ChainFloor2x2.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Floor2x2 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Floor2x2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Floor2x2.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SquareFloor */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_SquareFloor"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SquareFloor.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ChainFloor1x1 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_ChainFloor1x1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ChainFloor1x1.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FloorChain */ 
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_FloorChain"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FloorChain.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FloorBolt */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_FloorBolt"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FloorBolt.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_StreamVent */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_StreamVent"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/StreamVent.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PipeCurve */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_PipeCurve"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PipeCurve.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_JointPipe */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_JointPipe"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/JointPipe.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_T */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Pipe_T"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_T.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_Basic */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Pipe_Basic"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_Basic.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_Joint */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Pipe_Joint"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_Joint.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_Holder */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Pipe_Holder"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_Holder.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_Plate */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Pipe_Plate"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_Plate.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ProtpaneTank */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_ProtpaneTank"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ProtpaneTank.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ProtpaneTank01 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_ProtpaneTank01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/ProtpaneTank01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_T_Rotate */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Pipe_T_Rotate"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_T_Rotate.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Pipe_T_RotateFloor */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Pipe_T_RotateFloor"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Pipe_T_RotateFloor.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Crane */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Crane"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Crane.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Rail_00 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Rail_00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Rail_00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_RailSupport */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_RailSupport"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/RailSupportStrut.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Magnet */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Magnet"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Magnet.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_VentStair */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_VentStair"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/VentStair.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CrabDecal */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_CrabDecal"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/CrabDecal.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_WoodPlank */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_WoodPlank"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/WoodPlank.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*PinBall 가는 길 끝*/

	{ // 스태틱으로 갈수도 
		lstrcpy(m_szLoadingText, TEXT("Load Resources for Field : Model"));
		_matrix PreTransformMatrix = XMMatrixIdentity();

		/* For.Prototype_Component_Model_LargeKelpTree*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_LargeKelpTree"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/LargeKelpTree.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;


		/* For.Prototype_Component_Model_Large_Sharp_Cliff*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Large_Sharp_Cliff"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Large_Sharp_Cliff.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
			return E_FAIL;
		/* For.Prototype_Component_Model_DuneFence00*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_DuneFence00"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/DuneFence00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;
		/* For.Prototype_Component_Model_LifeRing*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_LifeRing"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/LifeRing.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;
		/* For.Prototype_Component_Model_Tank*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Tank"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Tank.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;
		/* For.Prototype_Component_Model_Tire*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Tire"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Tire.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;


		/* For.Prototype_Component_Model_Cooler*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_Cooler"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/Cooler.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;
		/* For.Prototype_Component_Model_CoolerTop*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_CoolerTop"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/CoolerTop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;



		/* For.Prototype_Component_Model_HoleTank*/
		PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_Component_Model_HoleTank"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/HoleTank.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
			return E_FAIL;
	}

#pragma endregion


#pragma region FIELD_PHYSICS
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Field : Physics"));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Terrain_Field",
		CPhysics_Terrain::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Terrain.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_VIBuffer_Terrain"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_Iron*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Iron",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Iron.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Iron"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_FeedersShop*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_FeedersShop",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/FeedersShop.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_FeedersShop"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BlackSmithShop*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_BlackSmithShop",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/BlackSmithShop.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_BlackSmithShop"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_PrawnShop*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_PrawnShop",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/PrawnShop.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_PrawnShop"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_CardBoardBuilding00*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_CardBoardBuilding00",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/CardBoardBuilding00.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_CardBoardBuilding00"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_CardBoardBuilding01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_CardBoardBuilding01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/CardBoardBuilding01.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_CardBoardBuilding01"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_CardBoardBuilding02*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_CardBoardBuilding02",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/CardBoardBuilding02.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_CardBoardBuilding02"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_CardBoardBuilding03*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_CardBoardBuilding03",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/CardBoardBuilding03.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_CardBoardBuilding03"))))
		return E_FAIL;
	/* For.Prototype_Component_Physics_Mesh_ButterTubTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_ButterTubTop",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/ButterTubTop.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_ButterTubTop"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ButterTub */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_ButterTub",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/ButterTub.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_ButterTub"))))
		return E_FAIL;

	///* For.Prototype_Component_Model_Spire */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Spire",
	//	CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Spire.physTriangle",
	//		LEVEL_FIELD, L"Prototype_Component_Model_Spire"))))
	//	return E_FAIL;

	/* For.Prototype_Component_Model_DuneFence00 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_DuneFence00",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/DuneFence00.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_DuneFence00"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_LifeRing */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_LifeRing",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/LifeRing.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_LifeRing"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Tank */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Tank",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Tank.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Tank"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_Tire */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Tire",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Tire.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Tire"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_Cooler */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Cooler",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Cooler.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Cooler"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_CoolerTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_CoolerTop",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/CoolerTop.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_CoolerTop"))))
		return E_FAIL;
	
	/* For.Prototype_Component_Model_HoleTank*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_HoleTank",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/HoleTank.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_HoleTank"))))
		return E_FAIL;
	
	/* For.Prototype_Component_Model_PinBallShip */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_PinBallShip",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/PinBallShip.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_PinBallShip"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Net */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Net",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Net.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Net"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ChainFloor2x2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_ChainFloor2x2",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/ChainFloor2x2.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_ChainFloor2x2"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Floor2x2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Floor2x2",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Floor2x2.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Floor2x2"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_SquareFloor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_SquareFloor",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/SquareFloor.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_SquareFloor"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ChainFloor1x1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_ChainFloor1x1",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/ChainFloor1x1.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_ChainFloor1x1"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FloorChain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_FloorChain",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/FloorChain.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_FloorChain"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_PipeCurve */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_PipeCurve",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/PipeCurve.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_PipeCurve"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_JointPipe */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_JointPipe",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/JointPipe.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_JointPipe"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pipe_T */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Pipe_T",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Pipe_T.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Pipe_T"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pipe_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Pipe_Basic",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Pipe_Basic.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Pipe_Basic"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pipe_Joint */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Pipe_Joint",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Pipe_Joint.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Pipe_Joint"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pipe_Holder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Pipe_Holder",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Pipe_Holder.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Pipe_Holder"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pipe_Plate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Pipe_Plate",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Pipe_Plate.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Pipe_Plate"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle00 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_SandCastle00",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/SandCastle00.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_SandCastle00"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_SandCastle01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/SandCastle01.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_SandCastle01"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SandCastle02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_SandCastle02",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/SandCastle02.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_SandCastle02"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ProtpaneTank */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_ProtpaneTank",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/ProtpaneTank.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_ProtpaneTank"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ProtpaneTank01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_ProtpaneTank01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/ProtpaneTank01.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_ProtpaneTank01"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pipe_T_Rotate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Pipe_T_Rotate",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Pipe_T_Rotate.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Pipe_T_Rotate"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pipe_T_RotateFloor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_Pipe_T_RotateFloor",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/Pipe_T_RotateFloor.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_Pipe_T_RotateFloor"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StreamVent */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_StreamVent",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/StreamVent.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_StreamVent"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_VentStair */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, L"Prototype_Component_Physics_Mesh_VentStair",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Field/VentStair.physTriangle",
			LEVEL_FIELD, L"Prototype_Component_Model_VentStair"))))
		return E_FAIL;
#pragma endregion


#pragma region FIELD_TEXTURE

	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, _T("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


#pragma region FIELD_GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Field : GameObject"));

#pragma region Enemies

	/*For. Prototype_GameObject_Sardine_Horn*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Sardine_Horn"),
		CSardine_Horn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Sardine_Tail*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Sardine_Tail"),
		CSardine_Tail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Sardine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_Sardine"),
		CEnemy_Sardine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Normie */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_Normie"),
		CEnemy_Normie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Gumpounder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_Gumpounder"),
		CEnemy_Gumpounder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Gumpounder_Hammer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Gumpounder_Hammer"),
		CGumpounder_Hammer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Bowman */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_Bowman"),
		CEnemy_Bowman::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Bowman_Arrow*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Bowman_Arrow"),
		CArrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Bowman_FakeArrow*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Bowman_FakeArrow"),
		CBowman_Arrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Bowman_RightClaw*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Bowman_RightClaw"),
		CBowman_RightClaw::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Normie_LeftClaw*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Normie_LeftClaw"),
		CNormie_LeftClaw::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Bowman_Band*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Bowman_Band"),
		CBowman_Band::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_BobbitWorm */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_BobbitWorm"),
		CEnemy_BobbitWorm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Splitter */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_Splitter"),
		CEnemy_Splitter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Splitter_Weapon*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Splitter_Weapon"),
		CSplitter_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Splitter_LeftClaw*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Splitter_LeftClaw"),
		CSplitter_LeftClaw::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_BobbitWorm_Mouth*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_BobbitWorm_Mouth"),
		CBobbitWorm_Mouth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_BobbitWorm_PlayerDetector*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_BobbitWorm_PlayerDetector"),
		CBobbitWorm_PlayerDetector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Boss_Pagurus*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Boss_Pagurus"),
		CBoss_Pagurus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Pagurus_Fork*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Pagurus_Fork"),
		CPagurus_Fork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Boss_Inkerton*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Boss_Inkerton"),
		CBoss_Inkerton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Inkerton_Gun*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Inkerton_Gun"),
		CInkerton_Gun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Inkerton_Bullet*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Inkerton_Bullet"),
		CInkerton_Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Inkerton_Nutcracker*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Inkerton_Nutcracker"),
		CInkerton_Nutcracker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Inkerton_StretchedNutcracker*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Inkerton_StretchedNutcracker"),
		CInkerton_StretchedNutcracker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Cuttlefish */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_Cuttlefish"),
		CEnemy_Cuttlefish::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Cuttlefish_Fist */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Cuttlefish_Fist"),
		CCuttlefish_Fist::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Cuttlefish_Tail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Cuttlefish_Tail"),
		CCuttlefish_Tail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Cuttlefish_GrabTentacle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Cuttlefish_GrabTentacle"),
		CCuttlefish_GrabTentacle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Cuttlefish_Missile */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Cuttlefish_Missile"),
		CCuttlefish_Missile::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/*For. Prototype_GameObject_Enemy_Frogfish */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_Frogfish"),
		CEnemy_Frogfish::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Manager*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Enemy_Manager"),
		CEnemy_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_RotatePipe */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_RotatePipe"),
		CRotatePipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_RotatePipeFloor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_RotatePipeFloor"),
		CRotatePipeFloor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_StreamVent */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_StreamVent"),
		CStreamVent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Magnet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_FIELD, TEXT("Prototype_GameObject_Magnet"),
		CMagnet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma endregion


	lstrcpyW(m_szLoadingText, TEXT("로딩끝."));
	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_PinBall()
{

#pragma region PINBALL_MODEL

	lstrcpy(m_szLoadingText, TEXT("모델 원형을 생성한다."));

	/*PinBall*/
	_matrix PreTransformMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_PinBallBarge */
	PreTransformMatrix = XMMatrixScaling(0.0005f, 0.0005f, 0.0005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_Component_Model_PinBallBarge"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/PinBall.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FlipL */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_Component_Model_FlipL"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FlipperL.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FlipR */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_Component_Model_FlipR"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FlipperR.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SpliterL */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_Component_Model_SpliterL"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SpliterL.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_SpliterR */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_Component_Model_SpliterR"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/SpliterR.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BumperTop */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_Component_Model_BumperTop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/BumperTop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BumperBottom */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_Component_Model_BumperBottom"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/BumperBottom.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/*PinBall_End*/

#pragma endregion


#pragma region PINBALL_PHYSICS
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Pinball : Physics"));


	/* For.Prototype_Component_Physics_Mesh_PinBallBarge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_PinBallBarge",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/PinBall/PinBallBarge.physTriangle",
			LEVEL_PINBALL, L"Prototype_Component_Model_PinBallBarge"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_FlipL */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_FlipL",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/PinBall/FlipL.physTriangle",
			LEVEL_PINBALL, L"Prototype_Component_Model_FlipL"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_FlipR */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_FlipR",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/PinBall/FlipR.physTriangle",
			LEVEL_PINBALL, L"Prototype_Component_Model_FlipR"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_SpliterL */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_SpliterL",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/PinBall/SpliterL.physTriangle",
			LEVEL_PINBALL, L"Prototype_Component_Model_SpliterL"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_SpliterR */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_SpliterR",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/PinBall/SpliterR.physTriangle",
			LEVEL_PINBALL, L"Prototype_Component_Model_SpliterR"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_BumperTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_BumperTop",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/PinBall/BumperTop.physTriangle",
			LEVEL_PINBALL, L"Prototype_Component_Model_BumperTop"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_BumperBottom */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PINBALL, L"Prototype_Component_Physics_Mesh_BumperBottom",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/PinBall/BumperBottom.physTriangle",
			LEVEL_PINBALL, L"Prototype_Component_Model_BumperBottom"))))
		return E_FAIL;

#pragma endregion


#pragma region PINBALL_GAME_OBJECT
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Pinball : GameObject"));

		/*PinBall*/
	/*For. Prototype_GameObject_PinBallBarge*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_PinBallBarge"),
		CPinBallBarge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Bumper*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_PinBall_Bumper"),
		CPinBall_Bumper::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Spliter*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_PinBall_Spliter"),
		CPinBall_Spliter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Spliter*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_PinBall_Spliter_L"),
		CPinBall_Spliter_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Flip*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_PinBall_Flip"),
		CPinBall_Flip::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_PinBall_Flip_L*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_PinBall_Flip_L"),
		CPinBall_Flip_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Monster

	/*For. Prototype_GameObject_Boss_Roland*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Boss_Roland"),
		CBoss_Roland::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Roland_Fist*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Roland_Fist"),
		CRoland_Fist::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Roland_Belly*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Roland_Belly"),
		CRoland_Belly::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Roland_Wrench_0_L*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Roland_Wrench_0_L"),
		CRoland_Wrench_0_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Roland_Wrench_1_L*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Roland_Wrench_1_L"),
		CRoland_Wrench_1_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Roland_Wrench_2_L*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Roland_Wrench_2_L"),
		CRoland_Wrench_2_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Roland_Wrench_0_R*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Roland_Wrench_0_R"),
		CRoland_Wrench_0_R::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Roland_Wrench_1_R*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Roland_Wrench_1_R"),
		CRoland_Wrench_1_R::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Roland_Wrench_2_R*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Roland_Wrench_2_R"),
		CRoland_Wrench_2_R::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Manager*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_Enemy_Manager"),
		CEnemy_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	/*For. Prototype_GameObject_NonMeshCollider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_PINBALL, TEXT("Prototype_GameObject_NonMeshCollider"),
		CNonMeshCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩 끝"));
	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_CrabTomb()
{

#pragma region CRABTOMB_VIBUFFER

	_matrix PreTransformMatrix = XMMatrixIdentity();
	
	/* For.Prototype_Component_Texture_CrabTombTerrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, _T("Prototype_Component_Texture_CrabTombTerrain"),
		CTexture::Create(m_pDevice, m_pContext, _T("../Bin/Resources/Textures/Terrain/CrabTombSandBaseColor%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CrabTomb_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Texture_CrabTomb_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/CrabTomb_Mask.png")))))
		return E_FAIL;

	lstrcpyW(m_szLoadingText, TEXT("Load Resources for CrabTomb : Model"));

	/* For.Prototype_Component_Model_BleachedKingMolt_01 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_BleachedKingMolt_01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BleachedKingMolt_02 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_BleachedKingMolt_02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BleachedKingMolt_03 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_BleachedKingMolt_03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BleachedKingMolt_04 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_BleachedKingMolt_04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_BleachedKingMolt_05 */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_BleachedKingMolt_05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/BleachedKingMolt_05.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_MoonBrick */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_MoonBrickFloor"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/MoonBrick.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_MoonBrickBar */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_MoonBrickBar"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/MoonBrickBar.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_MoonBrickPilarBroken */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_MoonBrickPilarBroken"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/MoonBrickPilarBroken.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_MoonBrickPilar */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_MoonBrickPilar"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/MoonBrickPilar.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_String */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_String"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/InkertonMap/String.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FeatherDuster */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_FeatherDuster"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Field00/FeatherDuster.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_FeatherDuster */
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_Model_FakeMoonBrick"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/NCCity/FakeMoonBrick.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region Enemy

	/*For. Prototype_GameObject_Boss_Inkerton*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Boss_Inkerton"),
		CBoss_Inkerton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Inkerton_Gun*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Inkerton_Gun"),
		CInkerton_Gun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Inkerton_Bullet*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Inkerton_Bullet"),
		CInkerton_Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Inkerton_Nutcracker*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Inkerton_Nutcracker"),
		CInkerton_Nutcracker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Inkerton_StretchedNutcracker*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Inkerton_StretchedNutcracker"),
		CInkerton_StretchedNutcracker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Boss_Petroch*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Boss_Petroch"),
		CBoss_Petroch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Petroch_SharpCrystal*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Petroch_SharpCrystal"),
		CPetroch_SharpCrystal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Petroch_CrystalSpawner*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Petroch_CrystalSpawner"),
		CPetroch_CrystalSpawner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Petroch_Shell*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Petroch_Shell"),
		CPetroch_Shell::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Petroch_Blockable*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Petroch_Blockable"),
		CPetroch_Blockable::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Petroch_LeftClaw*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Petroch_LeftClaw"),
		CPetroch_LeftClaw::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/*For. Prototype_GameObject_Enemy_Cuttlefish */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Enemy_Cuttlefish"),
		CEnemy_Cuttlefish::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Cuttlefish_Fist */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Cuttlefish_Fist"),
		CCuttlefish_Fist::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Cuttlefish_Tail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Cuttlefish_Tail"),
		CCuttlefish_Tail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Cuttlefish_GrabTentacle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Cuttlefish_GrabTentacle"),
		CCuttlefish_GrabTentacle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Cuttlefish_Missile */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Cuttlefish_Missile"),
		CCuttlefish_Missile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_BobbitWorm */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Enemy_BobbitWorm"),
		CEnemy_BobbitWorm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_BobbitWorm_Mouth*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_BobbitWorm_Mouth"),
		CBobbitWorm_Mouth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_BobbitWorm_PlayerDetector*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_BobbitWorm_PlayerDetector"),
		CBobbitWorm_PlayerDetector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Enemy_Manager*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_Enemy_Manager"),
		CEnemy_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region CRABTOMB_VIBUFFER

	lstrcpyW(m_szLoadingText, TEXT("Load Resources for CrabTomb : VIBuffer"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_Component_VIBuffer_Terrain_CrabTomb"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/MakeInkertonMap.bmp"), false))))
		return E_FAIL;

#pragma endregion


#pragma region CRABTOMB_PHYSICS

	/* For.Prototype_Component_Physics_Terrain_CrabTomb */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Terrain_CrabTomb",
		CPhysics_Terrain::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/CrabTomb_Terrain.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_VIBuffer_Terrain_CrabTomb"))))
		return E_FAIL;

#pragma region CRABTOMB_PHYSICS

	/* For.Prototype_Component_Physics_Mesh_BleachedKingMolt_01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_BleachedKingMolt_01",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/BleachedKingMolt_01.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_BleachedKingMolt_01"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_BleachedKingMolt_02*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_BleachedKingMolt_02",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/BleachedKingMolt_02.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_BleachedKingMolt_02"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_BleachedKingMolt_03*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_BleachedKingMolt_03",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/BleachedKingMolt_03.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_BleachedKingMolt_03"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_BleachedKingMolt_04*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_BleachedKingMolt_04",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/BleachedKingMolt_04.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_BleachedKingMolt_04"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_BleachedKingMolt_05*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_BleachedKingMolt_05",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/BleachedKingMolt_05.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_BleachedKingMolt_05"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_MoonBrickFloor*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_MoonBrickFloor",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/MoonBrickFloor.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_MoonBrickFloor"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_MoonBrickBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_MoonBrickBar",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/MoonBrickBar.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_MoonBrickBar"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_MoonBrickPilarBroken*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_MoonBrickPilarBroken",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/MoonBrickPilarBroken.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_MoonBrickPilarBroken"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_MoonBrickPilar*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_MoonBrickPilar",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/MoonBrickPilar.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_MoonBrickPilar"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_FakeMoonBrick*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Physics_Mesh_FakeMoonBrick",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/CrabTomb/FakeMoonBrick.physTriangle",
			LEVELID::LEVEL_CRABTOMB, L"Prototype_Component_Model_FakeMoonBrick"))))
		return E_FAIL;

#pragma endregion


#pragma region CRABTOMB_GAMEOBJECT

	/*For. Prototype_GameObject_CrabTomb_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_CrabTomb_Terrain"),
		CCrabTomb_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_FakeMoonBrick*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CRABTOMB, TEXT("Prototype_GameObject_FakeMoonBrick"),
		CFakeMoonBrick::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	lstrcpyW(m_szLoadingText, TEXT("로딩 끝"));
	m_bIsFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Beach()
{
#pragma region BEACH_SHADER

	lstrcpy(m_szLoadingText, TEXT("Load Resources for Beach: Shader"));
	/* For.Prototype_Component_Shader_WaterPlane */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_Shader_WaterPlane"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_WaterPlane.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region BEACH_MODEL

	wcscpy_s(m_szLoadingText, MAX_PATH, L"Load Resources for Beach: Texture");
	/* For.Prototype_Component_Texture_WaterNormal */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, L"Prototype_Component_Texture_WaterNormal",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/WaterCaustics/WaterNormal.jpg"))))
		return E_FAIL;

#pragma endregion

#pragma region BEACH_MODEL

	_matrix PreTransformMatrix = XMMatrixIdentity();
	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Beach : Model"));

	/* For.Prototype_Component_Model_Barnacle */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_Model_Barnacle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Barnacle.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Castle00 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_Model_Castle00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Castle00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Castle01 */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_Model_Castle01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Castle01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Chips */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_Model_Chips"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Chips.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_StarFish */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_Model_StarFish"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/StarFish.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Umbrella */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_Model_Umbrella"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Umbrella.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Seagull */
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_Model_Seagull"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Beach/Seagull.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

#pragma endregion


#pragma region BEACH_VIBUFFER

	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Beach : VIBuffer"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_Component_VIBuffer_Terrain_Beach"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/MakeTutorialMap.bmp"), false))))
		return E_FAIL;

#pragma endregion


#pragma region BEACH_PHYSICS

	/* For.Prototype_Component_Physics_Terrain_Beach */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, L"Prototype_Component_Physics_Terrain_Beach",
		CPhysics_Terrain::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Beach/Beach_Terrain.physTriangle",
			LEVELID::LEVEL_BEACH, L"Prototype_Component_VIBuffer_Terrain_Beach"))))
		return E_FAIL;

#pragma endregion

#pragma region BEACH_GAMEOBJECT

	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Beach : GameObject"));
	/*For. Prototype_GameObject_Beach_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_GameObject_Beach_Terrain"),
		CBeach_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Seagull*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_GameObject_Seagull"),
		CSeagull::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For. Prototype_GameObject_Seagull*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_GameObject_Fork"),
		CFork::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/*For. Prototype_GameObject_WaterPlane*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_GameObject_WaterPlane"),
		CWaterPlane::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Shark_Shell*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_BEACH, TEXT("Prototype_GameObject_Shark_Shell"),
		CNPC_Shark_Shell::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpyW(m_szLoadingText, TEXT("로딩 끝"));
	m_bIsFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Club()
{
#pragma region CLUB_MODEL

	_matrix PreTransformMatrix = XMMatrixIdentity();
	lstrcpyW(m_szLoadingText, TEXT("Load Resources for Club : Model"));

	/* For.Prototype_Component_Model_ClubCube*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_ClubCube"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubCube.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubMesh*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_ClubMesh"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubMesh.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubPipe00*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_ClubPipe00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubPipe00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubPipe01*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_ClubPipe01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubPipe01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_ClubPipe02*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_ClubPipe02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/ClubPipe02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Toilet*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_Toilet"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Toilet.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Radio*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_Radio"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Radio.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Tape*/
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_Tape"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Tape.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Stage*/
	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_Stage"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Stage.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Laser*/
	PreTransformMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_Component_Model_Laser"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Club/Laser.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
#pragma endregion


#pragma region CLUB_PHYSICS

	/* For.Prototype_Component_Physics_Mesh_Radio*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, L"Prototype_Component_Physics_Mesh_Radio",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Club/Radio.physTriangle",
			LEVELID::LEVEL_CLUB, L"Prototype_Component_Model_Radio"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_Tape*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, L"Prototype_Component_Physics_Mesh_Tape",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Club/Tape.physTriangle",
			LEVELID::LEVEL_CLUB, L"Prototype_Component_Model_Tape"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Mesh_ClubMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, L"Prototype_Component_Physics_Mesh_ClubMesh",
		CPhysics_Mesh::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Club/ClubMesh.physTriangle",
			LEVELID::LEVEL_CLUB, L"Prototype_Component_Model_ClubMesh"))))
		return E_FAIL;

#pragma endregion

#pragma region CLUB_GAMEOBJECT

	/*For. Prototype_GameObject_Radio*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_GameObject_Radio"),
		CRadio::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Laser*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_GameObject_Laser"),
		CLaser::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//lstrcpyW(m_szLoadingText, TEXT("Load Resources for Club : GameObject"));
	///*For. Prototype_GameObject_Beach_Terrain*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_CLUB, TEXT("Prototype_GameObject_Beach_Terrain"),
	//	CBeach_Terrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

#pragma endregion


	lstrcpyW(m_szLoadingText, TEXT("로딩 끝"));
	m_bIsFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Effects(LEVELID eLevelID)
{
	/* For.Prototype_Component_Texture_Tether */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Tether",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/tether.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TetherColor */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_TetherColor",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Tether_Color.png", 1))))
		return E_FAIL;

#pragma region PARTICLE TEX
	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Snow",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Snow/Snow.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_DefaultParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_DefaultParticle",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/Default_Particle.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line002 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line002",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/E_line_002c.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line004 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line004",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/E_line_004_clamp.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/FXT_line.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Plastic */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Plastic",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/Plastic.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PlasticDots */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_PlasticDots",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/FXT_dots.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line002d */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line002d",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/E_line_002d.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SparkFlash00 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SparkFlash00",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/SparkFlash00.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SparkFlash01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SparkFlash01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/SparkFlash01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SparkFlash02 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SparkFlash02",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/SparkFlash02.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SparkFlash03 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SparkFlash03",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/SparkFlash03.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SparkFlash04 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SparkFlash04",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/SparkFlash04.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SparkFlash05 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SparkFlash05",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/SparkFlash05.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HitFlash00 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_HitFlash00",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/HitFlash00.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HitFlash01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_HitFlash01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/HitFlash01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HitFlash02 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_HitFlash02",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/HitFlash02.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HitFlash03 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_HitFlash03",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/HitFlash03.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TearDrop */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_TearDrop",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/Teardrop.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MusicOn */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_MusicOn",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/MusicOn.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MusicQuater */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_MusicQuater",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/MusicQuater.png", 1))))
		return E_FAIL;

#pragma endregion


#pragma region SPRITE TEX
	/* For.Prototype_Component_Texture_Lightning */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Lightning",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Lightning.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RadialGlow */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_RadialGlow",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Radial_Glow.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_DustSmoke */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_DustSmoke",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/DustSmoke.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_EnergySpark1 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_EnergySpark1",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/EnergySpark1.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_EnergySpark03 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_EnergySpark03",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/EnergySpark03.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Spark */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Spark",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Spark.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Bokeh02 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Bokeh02",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Bokeh_02.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Flash01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Flash01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Flash_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Flash02 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Flash02",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Flash_02.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Impact04 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Impact04",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Impact_04.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Lightning04 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Lightning04",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Lightning_04.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Poof01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Poof01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Poof_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RadialGlow01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_RadialGlow01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_RadialGlow_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RadialGlow03 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_RadialGlow03",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_RadialGlow_03.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RadialGlow04 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_RadialGlow04",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_RadialGlow_04.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Shatter10 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Shatter10",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Shatter_10.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snap01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Snap01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Snap_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sparks01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Sparks01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Sparks_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sparks02 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Sparks02",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Sparks_02.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Splat01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Splat01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Splat_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Star01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Star01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Tex_Star_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HitSpark */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_HitSpark",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/HitSpark.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Smoke114 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Smoke114",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/E_smoke_114.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Smoke002 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Smoke002",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/E_smoke_002_a.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Smoke103 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Smoke103",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/E_smoke_103.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Glow005 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Glow005",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/Fx_glow_005.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark00 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark00",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark00.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark02 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark02",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark02.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark03 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark03",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark03.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark04 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark04",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark04.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark05 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark05",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark05.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark06 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark06",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark06.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark07 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark07",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark07.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark08 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark08",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark08.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark09 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark09",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark09.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark10 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark10",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark10.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark11 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark11",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark11.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark12 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark12",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark12.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GlowSpark13 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_GlowSpark13",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/GlowSpark13.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_FXT_trail01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_FXT_trail01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/FXT_trail01.png", 1))))
		return E_FAIL;
#pragma endregion


#pragma region MESHBASE 
	/* For.Prototype_Component_Texture_Vein032 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Vein032",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/E_vein_032.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Vein093 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Vein093",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/E_vein_093.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Vein138 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Vein138",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/E_vein_138.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail0158 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Trail0158",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_Trail_0158.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Evelyn002 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Evelyn002",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_trail_evelyn_002.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Noise017gt */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Noise017gt",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/noise_017_gt.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Trail01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Trail_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail02 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Trail02",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Trail_02.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail03 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Trail03",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Trail_03.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail10 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Trail10",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Trail_10.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Wind18 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Wind18",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Wind_18_add.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail06 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Trail06",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_Trail06.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Water */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Water",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_water.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Water001 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Water001",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_water_001.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Wind */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Wind",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_wind.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Wind26 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Wind26",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_Wind_26_alpha.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Wind29 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Wind29",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_Wind_29_alpha.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Pink */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Pink",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Pink.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail31 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Trail31",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Trail31.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line3 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line3",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Fx_line_3.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line04 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line04",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Fx_line_04_alpha.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line07 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line07",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Fx_line_07.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Boss10 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Boss10",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_BigBoss_sp_10.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Bite */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Bite",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_bite.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_LineSpot */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_LineSpot",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_line_Spot.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line001 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line001",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_line_001_alpha.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line008 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line008",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_line_004.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line012 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line012",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_line_012_c.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Balde001 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Balde001",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_Line_balde_001.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_LineC */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_LineC",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_line_c.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Line003 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Line003",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_line003.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SmallStarTrail2 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SmallStarTrail2",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_SmallStarTrail2.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Smoke001 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Smoke001",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_Smoke_001.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SmokeSlash */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SmokeSlash",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_smokeSlash.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sueyoi004 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Sueyoi004",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_sueyoi_skill_004.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail008 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Trail008",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/FXT_Trail_008.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ScaryFace */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_ScaryFace",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/ScaryFace.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_DNA */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_DNA",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/DNA.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MeshGlow137 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_MeshGlow137",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/MeshGlow_137.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MeshGlow001 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_MeshGlow001",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/E_glow_001.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Blade16 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Blade16",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Blade_16_alpha.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BloodCloud */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_BloodCloud",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Particle/Blood_cloud.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WaterSplash025 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_WaterSplash025",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/E_water_025.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_FXT_BubblesMany001 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_FXT_BubblesMany001",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Sprite/FXT_bubblesMany_001.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Eff_MetalScrap */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_Eff_MetalScrap",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/MetalScrap_basecolor.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MeshNoise01 */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_MeshNoise01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/Tex_Noise_01.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ButterflyFish */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_ButterflyFish",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/ButterflyFish.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SurgeonFish */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_Texture_SurgeonFish",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/MeshBase/SurgeonFish.png", 1))))
		return E_FAIL;

#pragma endregion

#pragma region SHADER

	/* For.Prototype_Component_Shader_VtxMesh_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxMesh_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh_Effect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPoint_GodRay */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxPoint_GodRay"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint_GodRay.hlsl"),
			VTXPOINT_PARTICLE::Elements, VTXPOINT_PARTICLE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxTrail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrail.hlsl"), VTXTRAIL::Elements, VTXTRAIL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPos_Rope */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxPos_Rope"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPos_Rope.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxMesh_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh_Particle.hlsl"), VTXMESH_PARTICLE::Elements, VTXMESH_PARTICLE::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region BUFFER

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_VIBuffer_Rope"),
		CVIBuffer_Rope::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, L"Prototype_Component_VIBuffer_MeshInstancing_SRT",
		CVIBuffer_Instancing_Mesh_SRT::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion



#pragma region MODEL
	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Blade"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Blade00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DNA"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/DNA.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_PlaneCircle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Plane_Circle.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_PlaneRect"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Plane_Rect.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Ring00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Ring00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.085f, 0.085f, 0.085f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Ring01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Ring01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Roll00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Roll00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Sphere00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Sphere00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Sphere01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Sphere01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Crystal

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_CrystalPart00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Crystal/Crystal_Part0.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_CrystalPart01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Crystal/Crystal_Part1.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_CrystalPart02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Crystal/Crystal_Part2.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Dome

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DomePlane00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome_Plane00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DomePlane01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome_Plane01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DomePlane02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome_Plane02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DomePlane03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome_Plane03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dome00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dome01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dome02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dome03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dome04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dome05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome05.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// MoonShell

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_EffMoonShell"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/MoonShell/MoonShell.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Barnacle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/MoonShell/Barnacle.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_MoonBrick"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/MoonShell/MoonBrick.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Screw

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Screw00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Screw/Screw00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Screw01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Screw/Screw01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Screw02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Screw/Screw02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Screw03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Screw/Screw03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Screw04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Screw/Screw04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Skill

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Hand"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Skill/Hand.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_ScaryFace"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Skill/ScaryFace.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_TopodaPunch"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Skill/Topoda_Punch.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Tentacle

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Tentacle00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Tentacle/Tentacle00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Tentacle01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Tentacle/Tentacle01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Tentacle02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Tentacle/Tentacle02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// MoonShell Effect

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_ShellWind00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/MoonShell/ShellWind00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_ShellWind01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/MoonShell/ShellWind01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Wind Impact

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_WindImpact"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/WindImpact.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Doom Screw

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DoomScrew"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Dome/Dome_Screw.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Impacts

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Impact"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Impact.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_WindImpact_Ground"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/WindImpact_Ground.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Cylinder

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_CylinderSteam"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Cylinder_Steam.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_CylinderDrop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Cylinder_Drop.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Magnet

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_MagnetTube"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Magnet_Tube.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_RustyMetal"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Rusty_Metal.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Crystal Shards

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_CrystalShard0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Crystal/CrystalShard.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_CrystalShard1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Crystal/CrystalShard1.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Skill

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Tornado"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Tornado00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_WaterSplash"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/WaterSplash.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Trail

	PreTransformMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_TrailHammer"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail_Hammer.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.075f, 0.075f, 0.075f) * XMMatrixRotationY(XMConvertToRadians(270.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail01.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail02.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail03.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail04.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail05.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail06"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail06.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail07"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail07.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail08"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail08.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail09"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail09.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail10"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail10.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail11"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail11.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail12"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail12.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Trail13"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail13.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_TrailGolf"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Trail_Golf.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_BrokenGlass00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Broken/Broken_Glass_00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_BrokenPiece00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Broken/Broken_Piece_00.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_WindCannon00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/FXMesh_wind_cannon02_001.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_WindCannon01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/FXMesh_wind_cannon02_003.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Daoguang05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Inkerton/daoguang_05.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Daoguang005"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Inkerton/Fx_mesh_daoguang005.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_BladeDoubleSide001"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Inkerton/FXMesh_blade_doubleside_001.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_BladeSingleSide001"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Inkerton/FXMesh_blade_singleside_001.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Blade001"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effects/Trail/Inkerton/FXT_mesh_blade_001.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/*------------------------------*/// Env Fish
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_ButterflyFish"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/EnvironmentObject/ButterflyFish.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_SurgeonFish"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/EnvironmentObject/SurgeonFish.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region EFFECT OBJECT : SPRITE
	/* For.Prototype_Effect_Sprite_Once */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_Sprite_Once"),
		CEffect_Sprite_Once::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_Sprite_Loop_NonBlend */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_Sprite_Loop_NonBlend"),
		CEffect_Sprite_Loop_NonBlend::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_Change_ExplToRiseW */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_Change_ExplToRiseW"),
		CEffect_Change_ExplToRiseW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_GodRay */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_GodRay"),
		CEffect_GodRay::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region EFFECT OBJECT : MESH
	/* For.Prototype_Effect_Mesh_Once */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_Mesh_Once"),
		CEffect_Mesh_Once::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_Mesh_Increase */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_Mesh_Increase"),
		CEffect_Mesh_Increase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_Mesh_ActivateShell */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_Mesh_ActivateShell"),
		CEffect_Mesh_ActivateShell::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_Mesh_ActivateShellWind */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_Mesh_ActivateShellWind"),
		CEffect_Mesh_ActivateShellWind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_RadialDistortion */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_RadialDistortion"),
		CEffect_RadialDistortion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_PlayerAttNormal */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_PlayerAttNormal"),
		CEffect_Player_AttNormal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_PlayerAttSwipe */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_PlayerAttSwipe"),
		CEffect_Player_AttSwipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_PlayerHammerOnce */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_PlayerHammerOnce"),
		CEffect_Player_HammerOnce::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_SplitterFear */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_SplitterFear"),
		CEffect_Enemy_Fear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_SplitterSwipe */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_SplitterSwipe"),
		CEffect_Splitter_Swipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_RolandIncrease */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_RolandIncrease"),
		CEffect_Roland_Increase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_RolandSlash */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_RolandSlash"),
		CEffect_Roland_Slash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_RolandBigCombo */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_RolandBigCombo"),
		CEffect_Roland_BigCombo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_RolandBigSwipe */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_RolandBigSwipe"),
		CEffect_Roland_BigSwipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_InkertonIncrease */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_InkertonIncrease"),
		CEffect_Inkerton_Increase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_InkertonScalingY */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_InkertonScalingY"),
		CEffect_Inkerton_ScalingY::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region EFFECT OBJECT : MESHINST
	/* For.Prototype_Effect_MeshInst_Once */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_MeshInst_Once"),
		CEffect_MeshInst_Once::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Effect_MeshInst_NonBlend */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Effect_MeshInst_NonBlend"),
		CEffect_MeshInst_NonBlend::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region EFFECT OBJECT : PART
	/* For.Prototype_GameObject_Effect_TexturedTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Effect_TexturedTrail"),
		CEffect_Textured_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_TexturedRope */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Effect_TexturedRope"),
		CEffect_Textured_Rope::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_ColoredTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Effect_ColoredTrail"),
		CEffect_Colored_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CLoader::Loading_Static_UI_Texture()
{
	/* For.Prototype_Component_Texture_Locked */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Locked"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Locked.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Round */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Round"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Round.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Triangle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Triangle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Triangle.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Shell_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_Shell_Logo",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Shell.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BlurryBox */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_BlurryBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/BlurryBox.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Clock */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_Clock",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Clock.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_X */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_X",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/X.png", 1))))
		return E_FAIL;

	/* Button =================================================================== */

	/* For.Prototype_Component_Texture_BlurryRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_BlurryRect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/BlurryRect.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/Button.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/Rect.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Button_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Button_Arrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/Arrow.png"), 1))))
		return E_FAIL;

	/* Compass =================================================================== */

	/* For.Prototype_Component_Texture_Compass */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Compass"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Compass/Compass.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CompassArrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_CompassArrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Compass/CompassArrow.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CompassArrow2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_CompassArrow2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Compass/CompassArrow2.dds"), 1))))
		return E_FAIL;

	/* Debuff ================================================================================= */

	/* For.Prototype_Component_Texture_Fear */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fear"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Debuff/Fear.png"), 1))))
		return E_FAIL;

	/* Dialogue =================================================================== */

	/* For.Prototype_Component_Texture_Dialogue_Box */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dialogue_Box"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/Dialogue Box.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Kril_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Kril_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/Kril/Kril_dialogue_%d.png"), 23))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Shark_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Shark_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Shark/NPC_Shark_dialogue_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Bablo_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Bablo_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Bablo/NPC_Bablo_dialogue.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Children_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Children_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Children/NPC_Children_dialogue_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Roland_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Roland_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Roland/NPC_Roland_dialogue.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Tortellini_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Tortellini_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Tortellini/NPC_Tortellini_dialogue.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Urchin_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Urchin_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Urchin/NPC_Urchin_dialogue.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_BlackSmith_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_BlackSmith_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_BlackSmith/NPC_BlackSmith_dialogue.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Chitan_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Chitan_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Chitan/NPC_Chitan_dialogue_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Nemma_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Nemma_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Nemma/NPC_Nemma_dialogue_%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Topoda_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Topoda_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Topoda/NPC_Topoda_dialogue.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Prawnathan_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Prawnathan_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Prawnathan/NPC_Prawnathan_dialogue.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_NPC_Firth_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Firth_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Firth/NPC_Firth_dialogue_%d.png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Crab_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Crab_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Crab/NPC_Crab_dialogue.png"), 1))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_NPC_Hermit_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Hermit_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Hermit/NPC_Hermit_dialogue.png"), 1))))
		return E_FAIL;

	/* Grapple ================================================================================== */

	/* For.Prototype_Component_Texture_Grapple */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grapple"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Grapple/Grapple_%d.png"), 2))))
		return E_FAIL;

	/* Item =================================================================== */

	/* For.Prototype_Component_Texture_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_Item",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Item/Item_%d.dds", 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ShopItem */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_ShopItem",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Item/ShopItem/Item_%d.dds", 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_QuestItem */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, L"Prototype_Component_Texture_QuestItem",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Item/QuestItem/Item_%d.dds", 1))))
		return E_FAIL;

	/* Menu ================================================================================== */

	/* For.Prototype_Component_Texture_MenuButton */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuButton.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuDot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuDot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuDot.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuGrass */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuGrass"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuGrass.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuHeader */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuHeader"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuHeader.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuStar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuStar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuStar.png"), 1))))
		return E_FAIL;

	/* MoonSnail ================================================================================== */

	/* For.Prototype_Component_Texture_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Arrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/Arrow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Decoration */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Decoration"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/Decoration.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MoonsnailFilled */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MoonsnailFilled"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/MoonsnailFilled.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MoonsnailOutline */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_MoonsnailOutline"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/MoonsnailOutline.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PrevBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_PrevBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/PrevBorder.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PrevFilled */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_PrevFilled"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/PrevFilled.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RectBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_RectBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/RectBorder.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Star */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/Star.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Star_8Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_8Point"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/Star8Point.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WindowBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_WindowBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/WindowBorder.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WindowFilled */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_WindowFilled"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/WindowFilled.png"), 1))))
		return E_FAIL;

	/* Notice ================================================================================== */

	/* For.Prototype_Component_Texture_Notice */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Notice"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Notice/NoticeBorder.dds"), 1))))
		return E_FAIL;

	/* NPC ================================================================================== */

	/* For.Prototype_Component_Texture_ChatBubble */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChatBubble"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/NPC/ChatBubble_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_LocationIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_LocationIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/NPC/LocationIcon_%d.png"), 3))))
		return E_FAIL;

	/* Peril ================================================================================== */

	/* For.Prototype_Component_Texture_Peril */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Peril"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Peril/Peril_%d.png"), 2))))
		return E_FAIL;

	/* PlayerUI =================================================================== */

	/* For.Prototype_Component_Texture_CurrencyBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_CurrencyBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/CurrencyBorder.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ghost */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ghost"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Ghost.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HeartKelpCount */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_HeartKelpCount"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/HeartKelpCount.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HookCount */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_HookCount"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/HookCount.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HurtBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_HurtBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/HurtBorder.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_IconBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_IconBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/IconBorder.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_IconBorder2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_IconBorder2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/IconBorder2.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Ring.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Microplastic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Microplastic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Microplastic.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TechniqueBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_TechniqueBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/TechniqueBorder.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Umami */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Umami"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Umami_%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UmamiWhite */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_UmamiWhite"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/UmamiWhite.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RemainLocationIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_RemainLocationIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/RemainLocationIcon.dds"), 1))))
		return E_FAIL;

	/* SheleportPrev ================================================================================== */

	/* For.Prototype_Component_Texture_SheleportPrev */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_SheleportPrev"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/SheleportPrev/SheleportPrev_%d.png"), 8))))
		return E_FAIL;

	/* Shell ================================================================================== */

	/* For.Prototype_Component_Texture_Shell_Def */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Shell_Def"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shell/Def.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Shell_Weight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Shell_Weight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shell/weight.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Shell */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Shell"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shell/Shell_%d.png"), 5))))
		return E_FAIL;

	/* Skill ================================================================================== */

	/* For.Prototype_Component_Texture_Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Skill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill/Skill_%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SkillChoice */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillChoice"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill/SkillChoice.png"), 1))))
		return E_FAIL;

	/* Sprite ================================================================================== */

	/* For.Prototype_Component_Texture_Keyboard */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Keyboard"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Sprite/Keyboard.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CuteKeyboard */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_CuteKeyboard"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Sprite/CuteKeyboard.dds"), 1))))
		return E_FAIL;

	/* Stat ================================================================================== */

	/* For.Prototype_Component_Texture_StatIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stat/Icon_%d.png"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ExtraIcons */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_ExtraIcons"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stat/ExtraIcons.png"), 1))))
		return E_FAIL;

	/* Stretch ================================================================================== */

	/* For.Prototype_Component_Texture_BarBorder_Armor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_BarBorder_Armor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/BarBorder_Armor.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BarBorder_Health */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_BarBorder_Health"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/BarBorder_Health.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BarBoss */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_BarBoss"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/BarBoss.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RampHp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_RampHp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/RampHp.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RampShell */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_RampShell"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/RampShell.dds"), 1))))
		return E_FAIL;

	/* Technique ================================================================================== */

	/* For.Prototype_Component_Texture_Technique */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Technique"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Technique/Tech_%d.png"), 4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Static_UI_GameObject()
{
	/* Canvas ================================================================================== */

	/*For. Prototype_GameObject_UI_Boss*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Boss"),
		CUI_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Description*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Description"),
		CUI_Description::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Dialogue*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Dialogue"),
		CUI_Dialogue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Inventory*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Inventory"),
		CUI_Inventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Menu*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Menu"),
		CUI_Menu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Monster*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Monster"),
		CUI_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_MoonSnail*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_MoonSnail"),
		CUI_MoonSnail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Sheleport*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Sheleport"),
		CUI_Sheleport::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Stat*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Stat"),
		CUI_Stat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Status*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Status"),
		CUI_Status::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_ShellDesc*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_ShellDesc"),
		CUI_ShellDesc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Shop*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Shop"),
		CUI_Shop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_ShopButton*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_ShopButton"),
		CUI_ShopButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_ShopSelect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_ShopSelect"),
		CUI_ShopSelect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Player"),
		CUI_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_PlayerLeft*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_PlayerLeft"),
		CUI_PlayerLeft::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_PlayerRight*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_PlayerRight"),
		CUI_PlayerRight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_PlayerShellDesc*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_PlayerShellDesc"),
		CUI_PlayerShellDesc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_GainSkill*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_GainSkill"),
		CUI_GainSkill::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Game*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Game"),
		CUI_Game::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Debuff*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Debuff"),
		CUI_Debuff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Option*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Option"),
		CUI_Option::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* UI ================================================================================== */

	/*For. Prototype_GameObject_UI_BallanceBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_BallanceBar"),
		CUI_BallanceBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_BallanceBorder*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_BallanceBorder"),
		CUI_BallanceBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Button*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Button"),
		CUI_Button::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_ChatBubble*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_ChatBubble"),
		CUI_ChatBubble::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Compass*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Compass"),
		CUI_Compass::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_CompassArrow*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_CompassArrow"),
		CUI_CompassArrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/*For. Prototype_GameObject_UI_Credit*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Credit"),
		CUI_Credit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_DialogueBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_DialogueBox"),
		CUI_DialogueBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/*For. Prototype_GameObject_UI_DynamicButton*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_DynamicButton"),
		CUI_DynamicButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_DynamicTexture*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_DynamicTexture"),
		CUI_DynamicTexture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_DefaultStretch*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_DefaultStretch"),
		CUI_DefaultStretch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_FixStretch*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_FixStretch"),
		CUI_FixStretch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_FixStretchBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_FixStretchBar"),
		CUI_FixStretchBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_FixStretchBarEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_FixStretchBarEffect"),
		CUI_FixStretchBarEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Ghost*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Ghost"),
		CUI_Ghost::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Grapple*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Grapple"),
		CUI_Grapple::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Finish*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Finish"),
		CUI_Finish::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Hide*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Hide"),
		CUI_Hide::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Hurt*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Hurt"),
		CUI_Hurt::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Peril*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Peril"),
		CUI_Peril::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Interaction*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Interaction"),
		CUI_Interaction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Line*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Line"),
		CUI_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_LocationIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_LocationIcon"),
		CUI_LocationIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Lock*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Lock"),
		CUI_Lock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Locked*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Locked"),
		CUI_Locked::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Notice*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Notice"),
		CUI_Notice::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Ring*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Ring"),
		CUI_Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Shake*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Shake"),
		CUI_Shake::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_StatButton*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_StatButton"),
		CUI_StatButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_StatusButton*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_StatusButton"),
		CUI_StatusButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Speaker*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Speaker"),
		CUI_Speaker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Shell*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Shell"),
		CUI_Shell::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_ShellBubble*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_ShellBubble"),
		CUI_ShellBubble::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_SkillEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_SkillEffect"),
		CUI_SkillEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Stretch*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Stretch"),
		CUI_Stretch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_StretchBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_StretchBar"),
		CUI_StretchBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_StretchBarEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_StretchBarEffect"),
		CUI_StretchBarEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Umami*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Umami"),
		CUI_Umami::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Umami*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_UmamiEffect"),
		CUI_UmamiEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Value*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Value"),
		CUI_Value::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_UI_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect"),
		CUI_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CLoader::Loading_CameraTool(LEVELID eNextLevelID)
{
	/*For. Prototype_GameObject_CameraTool*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_CameraTool"),
		CCameraTool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_CineCameraTool*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_CineCameraTool"),
		CCineCameraTool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_CineDebugTool*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_CineDebugTool"),
		CCineDebugTool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}
#endif

HRESULT CLoader::Loading_Player(LEVELID eNextLevelID)
{
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : Player"));

	_matrix PreTransformMatrix = XMMatrixIdentity();

#pragma region MODEL

	/* For.Prototype_Component_Model_Player */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Player/Player/Player.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Topoda */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Topoda"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Skill/Topoda/Topoda.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/*For. Prototype_Component_Model_Urchin*/
	PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Urchin_Skill"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Skill/Urchin/Urchin.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region PARTOBJECT

	/*For. Prototype_GameObject_Camera_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_CineCamera_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_CineCamera_Player"),
		CCineCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_SpringArm_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_SpringArm_Player"),
		CSpringArm_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Weapon_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Weapon_Player"),
		CWeapon_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Hand_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Hand_Player"),
		CHand_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_ActiveSkill_MantisPunch*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_ActiveSkill_MantisPunch"),
		CActiveSkill_MantisPunch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_ActiveSkill_Urchin*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_ActiveSkill_Urchin"),
		CActiveSkill_Urchin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_ActiveSkill_BobbitTrap*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_ActiveSkill_BobbitTrap"),
		CActiveSkill_BobbitTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Skill_Topoda*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Skill_Topoda"),
		CSkill_Topoda::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region GAMEOBJECT

	/*For. Prototype_GameObject_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_StateMachine_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_StateMachine_Player"),
		CStateMachine_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_StateMachine_PlayerUpper */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_StateMachine_PlayerUpper"),
		CStateMachine_PlayerUpper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region MANAGER

	CPlayerInstance* pPlayerInstance = CPlayerInstance::Create();
	if (pPlayerInstance == nullptr)
		return E_FAIL;

	CPlayerManager::Init(pPlayerInstance);

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_Equipment(LEVELID eNextLevelID)
{
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : Equipment"));

	_matrix PreTransformMatrix = XMMatrixIdentity();

#pragma region MODEL

	/* For.Prototype_Component_Model_Fork */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Fork"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/Fork/Fork.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_MiddleFork */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_MiddleFork"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/MiddleFork/MiddleFork.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_GoldFork */
	PreTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_GoldFork"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/GoldFork/GoldFork.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Shell_Home */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Shell_Home"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Shell/Home/Home.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Shell_SodaCan */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Shell_SodaCan"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Shell/SodaCan/SodaCan.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Shell_Duck */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Shell_Duck"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Shell/Duck/Duck.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Shell_MirrorBall */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Shell_MirrorBall"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Shell/MirrorBall/MirrorBall.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Shell_MirrorBall_Roll */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, -0.3f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Shell_MirrorBall_Roll"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Shell/MirrorBall/MirrorBall.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Shell_TennisBall */
	PreTransformMatrix = XMMatrixScaling(0.0025f, 0.0025f, 0.0025f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Shell_TennisBall"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Shell/TennisBall/TennisBall.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Shell_TennisBall_Roll */
	PreTransformMatrix = XMMatrixScaling(0.0025f, 0.0025f, 0.0025f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, -0.35f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Shell_TennisBall_Roll"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Shell/TennisBall/TennisBall.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

#pragma endregion

#pragma region PHYSICS

	/* For.Prototype_Component_Model_Shell_Home */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, L"Prototype_Component_Physics_Convex_Shell_Home",
		CPhysics_Convex::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Shell/Shell_Home.physConvex",
			eNextLevelID, L"Prototype_Component_Model_Shell_Home"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Physcis_Convex_Shell_SodaCan */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, L"Prototype_Component_Physics_Convex_Shell_SodaCan",
		CPhysics_Convex::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Shell/Shell_SodaCan.physConvex",
			eNextLevelID, L"Prototype_Component_Model_Shell_SodaCan"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Convex_Shell_MirrorBall */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, L"Prototype_Component_Physics_Convex_Shell_MirrorBall",
		CPhysics_Convex::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Shell/Shell_MirrorBall.physConvex",
			eNextLevelID, L"Prototype_Component_Model_Shell_MirrorBall"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Physcis_Convex_Shell_Duck */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, L"Prototype_Component_Physics_Convex_Shell_Duck",
		CPhysics_Convex::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Shell/Shell_Duck.physConvex",
			eNextLevelID, L"Prototype_Component_Model_Shell_Duck"))))
		return E_FAIL;

	/* For.Prototype_Component_Physics_Convex_Shell_TennisBall */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, L"Prototype_Component_Physics_Convex_Shell_TennisBall",
		CPhysics_Convex::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Physics/Shell/Shell_TennisBall.physConvex",
			eNextLevelID, L"Prototype_Component_Model_Shell_TennisBall"))))
		return E_FAIL;

#pragma endregion

#pragma region COMPONENT

	/*For. Prototype_Component_ShellSkill_Fizzle*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_ShellSkill_Fizzle"),
		CShellSkill_Fizzle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_Component_ShellSkill_RollOut*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_ShellSkill_RollOut"),
		CShellSkill_RollOut::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region GAMEOBEJCT

	/*For. Prototype_GameObject_Shell_World*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Shell_World"),
		CShell_World::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Shell_Part*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Shell_Part"),
		CShell_Part::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_Item(LEVELID eNextLevelID)
{
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : Item"));

	_matrix PreTransformMatrix = XMMatrixIdentity();

#pragma region MODEL

	/* For.Prototype_Component_Model_Item_HeartKelp */
	PreTransformMatrix = XMMatrixScaling(0.0075f, 0.0075f, 0.0075f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Item_HeartKelp"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Item/HeartKelp/HeartKelp.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_BottleCap */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Item_BottleCap"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Item/BottleCap/BottleCap.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix, true))))
		return E_FAIL;

#pragma endregion

#pragma region GAMEOBEJCT

	/*For. Prototype_GameObject_Item_HeartKelp*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Item_HeartKelp"),
		CItem_HeartKelp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Item_BottleCap*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Item_BottleCap"),
		CItem_BottleCap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_Monster(LEVELID eNextLevelID)
{
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : Monster"));

#pragma region STATIC_FSM	

	/* For.Prototype_Component_Sardine_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Sardine_StateMachine"),
		CSardine_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Bowman_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Bowman_StateMachine"),
		CBowman_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_BobbitWorm_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_BobbitWorm_StateMachine"),
		CBobbitWorm_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Splitter_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Splitter_StateMachine"),
		CSplitter_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Inkerton_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Inkerton_StateMachine"),
		CInkerton_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Pagurus_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Pagurus_StateMachine"),
		CPagurus_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Inkerton_EncounterStateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Inkerton_EncounterStateMachine"),
		CInkerton_EncounterStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Roland_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Roland_StateMachine"),
		CRoland_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Petroch_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Petroch_StateMachine"),
		CPetroch_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Frogfish_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Frogfish_StateMachine"),
		CFrogfish_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Normie_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Normie_StateMachine"),
		CNormie_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Gumpounder_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Gumpounder_StateMachine"),
		CGumpounder_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Cuttlefish_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Cuttlefish_StateMachine"),
		CCuttlefish_StateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region MODEL

	_matrix PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Enemy_Sardine */
	{
		_matrix PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Enemy_Sardine"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Enemy_Sardine/Enemy_Sardine.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Enemy_Normie */
	{
		PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(155.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Enemy_Normie"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Enemy_Normie/Enemy_Normie.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Enemy_Gumpounder */
	{
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Enemy_Gumpounder"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Enemy_Gumpounder/Enemy_Gumpounder.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Enemy_Bowman */
	{
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Enemy_Bowman"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Enemy_Bowman/Enemy_Bowman.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Bowman_Arrow */
	{
		PreTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Bowman_Arrow"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/Arrow/Bowman_Arrow.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Enemy_BobbitWorm*/
	{
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(-70.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Enemy_BobbitWorm"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Enemy_BobbitWorm/Enemy_BobbitWorm.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Enemy_Spliter*/
	{
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Enemy_Splitter"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Enemy_Splitter/Enemy_Splitter.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Boss_Petroch*/
	{
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(-45.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Boss_Petroch"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Boss_Petroch/Boss_Petroch.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Petroch_Shell*/
	{
		//* XMMatrixRotationY(XMConvertToRadians(-45.f)) * XMMatrixRotationX(XMConvertToRadians(-80.f)) * XMMatrixRotationZ(XMConvertToRadians(-270.f))
		PreTransformMatrix = XMMatrixScaling(0.0057f, 0.0057f, 0.0057f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Petroch_Shell"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Shell/Petroch_Shell/Petroch_Shell.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Boss_Petroch_Crystal*/
	{
		PreTransformMatrix = XMMatrixScaling(0.6f, 0.6f, 0.6f);
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Petroch_Crystal"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Boss_Petroch_Crystal/Petroch_Crystal.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Boss_Inkerton*/
	{
		//* XMMatrixRotationY(XMConvertToRadians(-45.f)) * XMMatrixRotationX(XMConvertToRadians(-80.f)) * XMMatrixRotationZ(XMConvertToRadians(-270.f))
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Boss_Inkerton"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Boss_Inkerton/Boss_Inkerton.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Boss_Roland*/
	{
		//* XMMatrixRotationY(XMConvertToRadians(-45.f)) * XMMatrixRotationX(XMConvertToRadians(-80.f)) * XMMatrixRotationZ(XMConvertToRadians(-270.f))
		PreTransformMatrix = XMMatrixScaling(0.0055f, 0.0055f, 0.0055f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Boss_Roland"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Boss_Roland/Boss_Roland.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Boss_Pagurus*/
	{
		//* XMMatrixRotationY(XMConvertToRadians(-45.f)) * XMMatrixRotationX(XMConvertToRadians(-80.f)) * XMMatrixRotationZ(XMConvertToRadians(-270.f))
		PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Boss_Pagurus"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Boss_Pagurus/Boss_Pagurus.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Enemy_Cuttlefish */
	{
		_matrix PreTransformMatrix = XMMatrixScaling(0.00006f, 0.00006f, 0.00006f) * XMMatrixRotationY(XMConvertToRadians(155.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Enemy_Cuttlefish"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Enemy_Cuttlefish/Enemy_Cuttlefish.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Enemy_Frogfish */
	{
		_matrix PreTransformMatrix = XMMatrixScaling(0.006f, 0.006f, 0.006f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Enemy_Frogfish"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/Enemy/Enemy_Frogfish/Enemy_Frogfish.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
			return E_FAIL;
	}

	/* For.Prototype_Component_Model_Roland_Wrench_0_R*/
	{		
		PreTransformMatrix = XMMatrixScaling(0.0055f, 0.0055f, 0.0055f) * XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Roland_Wrench_0_R"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/Wrench/Roland_Wrench_0_R.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Roland_Wrench_1_R*/
	{
		PreTransformMatrix = XMMatrixScaling(0.0055f, 0.0055f, 0.0055f) * XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Roland_Wrench_1_R"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/Wrench/Roland_Wrench_1_R.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Roland_Wrench_2_R*/
	{
		PreTransformMatrix = XMMatrixScaling(0.0055f, 0.0055f, 0.0055f) * XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Roland_Wrench_2_R"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/Wrench/Roland_Wrench_2_R.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Roland_Wrench_0_L*/
	{
		PreTransformMatrix = XMMatrixScaling(0.0055f, 0.0055f, 0.0055f) * XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Roland_Wrench_0_L"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/Wrench/Roland_Wrench_0_L.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Roland_Wrench_1_L*/
	{
		PreTransformMatrix = XMMatrixScaling(0.0055f, 0.0055f, 0.0055f) * XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Roland_Wrench_1_L"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/Wrench/Roland_Wrench_1_L.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
	/* For.Prototype_Component_Model_Roland_Wrench_2_L*/
	{
		PreTransformMatrix = XMMatrixScaling(0.0055f, 0.0055f, 0.0055f) * XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Roland_Wrench_2_L"),
			CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Equipment/Weapon/Wrench/Roland_Wrench_2_L.fbx", CModel::TYPE::NON_ANIM, PreTransformMatrix))))
			return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_NPC(LEVELID eNextLevelID)
{
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : NPC"));

	_matrix PreTransformMatrix = XMMatrixIdentity();

#pragma region MODEL

	/* For.Prototype_Component_Model_NPC0 */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/NPCs/NPC0.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC1 */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/NPCs/NPC1.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC2 */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/NPCs/NPC2.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC3 */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/NPCs/NPC3.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC4 */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC4"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/NPCs/NPC4.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC5 */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC5"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/NPCs/NPC5.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homeless */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_Homeless"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Homeless/Homeless.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Bablo */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Bablo"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Bablo/Bablo.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_BlackSmith */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_BlackSmith"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/BlackSmith/BlackSmith.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Chitan */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Chitan"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Chitan/Chitan.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Nemma */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Nemma"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Nemma/NPC_Nemma.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Pollip */
	PreTransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Pollip"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Pollip/Pollip.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Prawnathan */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Prawnathan"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Prawnathan/Prawnathan.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Sessi */
	PreTransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Sessi"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Sessi/Sessi.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Shark */
	PreTransformMatrix = XMMatrixScaling(0.0000003f, 0.0000003f, 0.0000003f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Shark"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Shark/Shark.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Topoda */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Topoda"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Topoda/Topoda.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Tortellini */
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Tortellini"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Tortellini/Tortellini.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Urchin */
	PreTransformMatrix = XMMatrixScaling(0.0002f, 0.0002f, 0.0002f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Urchin"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Urchin/Urchin.fbx", CModel::TYPE::PRE_TRANSFORM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_DanceCrab*/
	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_DanceCrab"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/DanceCrab/DanceCrab.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_ShellSplitter*/
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_ShellSplitter"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/ShellSplitter/ShellSplitter.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Roland_Death*/
	PreTransformMatrix = XMMatrixScaling(0.0045f, 0.0045f, 0.0045f);
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Roland_Death"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Roland_Death/Roland.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Roland*/
	PreTransformMatrix = XMMatrixScaling(0.0045f, 0.0045f, 0.0045f);
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Roland"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Roland/Roland.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Inkerton*/
	PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Inkerton"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Inkerton/Inkerton.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Firth*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Firth"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Club_Firth/Player.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Player*/
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Player"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Club_Player/Player.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Chitan_Fight*/
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Chitan_Fight"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Chitan_Fight/Chitan_Fight.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Inkerton_Fight*/
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_Component_Model_NPC_Inkerton_Fight"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Character/NPC/Inkerton_Fight/Inkerton_Fight.fbx", CModel::TYPE::ANIM, PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region GAMEOBJECT

	/*For. Prototype_GameObject_NPC_Bablo*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Bablo"),
		CNPC_Bablo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_BlackSmith*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_BlackSmith"),
		CNPC_BlackSmith::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Chitan*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Chitan"),
		CNPC_Chitan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Nemma*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Nemma"),
		CNPC_Nemma::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Children*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Children"),
		CNPC_Children::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Prawnathan*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Prawnathan"),
		CNPC_Prawnathan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Shark*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Shark"),
		CNPC_Shark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Topoda*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Topoda"),
		CNPC_Topoda::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Tortellini*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Tortellini"),
		CNPC_Tortellini::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Urchin*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Urchin"),
		CNPC_Urchin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_DanceCrab*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_DanceCrab"),
		CNPC_DanceCrab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_ShellSplitter*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_ShellSplitter"),
		CNPC_ShellSplitter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Roland_Death*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Roland_Death"),
		CNPC_Roland_Death::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Roland*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Roland"),
		CNPC_Roland::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Firth*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Firth"),
		CNPC_Firth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_GuardCrab*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_GuardCrab"),
		CNPC_GuardCrab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Player"),
		CNPC_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Fighter*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Fighter"),
		CNPC_Fighter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Roland_Final*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Roland_Final"),
		CNPC_Roland_Final::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Inkerton_Final*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Inkerton_Final"),
		CNPC_Inkerton_Final::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_Interactive */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_Interactive"),
		CNPC_Interactive::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_NPC_NonInteractive */
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_NPC_NonInteractive"),
		CNPC_NonInteractive::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_Projectile(LEVELID eNextLevelID)
{
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : Projectile"));

#pragma region GAMEOBJECT

	/*For. Prototype_GameObject_ProjectileManager*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Projectile_Manager"),
		CProjectile_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Projectile_Fizzle*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Projectile_Fizzle"),
		CProjectile_Fizzle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Projectile_Grapple*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Projectile_Grapple"),
		CProjectile_Grapple::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Projectile_Urchin*/
	if (FAILED(m_pGameInstance->Add_Prototype(eNextLevelID, TEXT("Prototype_GameObject_Projectile_Urchin"),
		CProjectile_Urchin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_DummyObject(LEVELID eLevelID)
{
	/*For. Prototype_GameObject_Dummy_NonAnim*/
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dummy_NonAnim"),
		CDummy_NonAnim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For. Prototype_GameObject_Dummy_Anim */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dummy_Anim"),
		CDummy_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_FileData()
{
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static : FileData"));

	// Dialogue File
	CService_Dialogue* pDialogueService = CService_Dialogue::Create();
	CFileData_Locator<FDialogueData>::Register_FileData(pDialogueService);

	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Beach/Shark.txt");

	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Village/Bablo.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Village/Children.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Village/Roland.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Village/Tortellini.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Village/Urchin.txt");

	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Field/BlackSmith.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Field/Chitan.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Field/Nemma.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Field/Prawnathan.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Field/Topoda.txt");

	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Club/Firth.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Club/GuardCrab.txt");
	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Club/Player.txt");

	pDialogueService->Add_Path(L"../Bin/DataFiles/File_Data/Dialogue_Data/Beach/Kril.txt");
	if (FAILED(pDialogueService->Load_Data()))
		return E_FAIL;

	// Item File
	CService_Item* pItemService = CService_Item::Create();
	CFileData_Locator<FItemData>::Register_FileData(pItemService);

	pItemService->Add_Path(L"../Bin/DataFiles/File_Data/Item_Data/Item.txt");
	pItemService->Add_Path(L"../Bin/DataFiles/File_Data/Item_Data/QuestItem.txt");
	pItemService->Add_Path(L"../Bin/DataFiles/File_Data/Item_Data/ShopItem.txt");
	pItemService->Add_Path(L"../Bin/DataFiles/File_Data/Item_Data/ShopItemNPC.txt");

	if (FAILED(pItemService->Load_Data()))
		return E_FAIL;


	// Skill Data
	CService_Skill* pSkillService = CService_Skill::Create();
	CFileData_Locator<FSkillData>::Register_FileData(pSkillService);

	pSkillService->Add_Path(L"../Bin/DataFiles/File_Data/Skill_Data/Active_Skill.txt");
	pSkillService->Add_Path(L"../Bin/DataFiles/File_Data/Skill_Data/Passive_Skill.txt");

	if (FAILED(pSkillService->Load_Data()))
		return E_FAIL;


	// Sheleport Data
	CService_Sheleport* pSheleportService = CService_Sheleport::Create();
	CFileData_Locator<FSheleportData>::Register_FileData(pSheleportService);

	pSheleportService->Add_Path(L"../Bin/DataFiles/File_Data/Sheleport_Data/Sheleport.txt");
	if (FAILED(pSheleportService->Load_Data()))
		return E_FAIL;


	// Shell Data
	CService_Shell* pShellService = CService_Shell::Create();
	CFileData_Locator<FShellDescData>::Register_FileData(pShellService);

	pShellService->Add_Path(L"../Bin/DataFiles/File_Data/Shell_Data/Shell.txt");

	if (FAILED(pShellService->Load_Data()))
		return E_FAIL;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
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
