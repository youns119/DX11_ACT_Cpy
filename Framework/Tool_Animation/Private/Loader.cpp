#include "pch.h"
#include "Loader.h"

#include "GameInstance.h"

#include "Sky.h"
#include "Terrain.h"
#include "ToolCamera.h"
#include "AnimObject.h"
#include "ToolPartObject.h"
#include "CineCamera_Tool.h"
#include "CameraDebug_Tool.h"

#include "ImGui_Manager.h"

CLoader::CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice{ _pDevice }
	, m_pContext{ _pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

_uint WINAPI Thread_Main(void* _pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(_pArg);

	if (FAILED(pLoader->Start_Loading()))
		MSG_BOX("Failed to Loading");

	return 0;
}

HRESULT CLoader::Init(LEVELID _eNextLevelID)
{
	m_eNextLevelID = _eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (m_hThread == 0)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start_Loading()
{
	HRESULT hr{};

	hr = CoInitializeEx(nullptr, 0);
	if (FAILED(hr))
		return hr;

	EnterCriticalSection(&m_CriticalSection);

	switch (m_eNextLevelID)
	{
	case LEVELID::LEVEL_TOOL:
		hr = Loading_For_Level_Tool();
		break;
	default:
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

void CLoader::SetUp_WindowText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

#endif

HRESULT CLoader::Loading_For_Level_Tool()
{
	// Shader
	{
		// Shader_VtxMesh
		lstrcpyW(m_szLoadingText, TEXT("Shader_VtxMesh Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC,
				TEXT("Shader_VtxMesh"),
				CShader::Create
				(
					m_pDevice,
					m_pContext,
					TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"),
					VTXMESH::Elements,
					VTXMESH::iNumElements
				)
			)))
			return E_FAIL;

		// Shader_VtxAnimMesh
		lstrcpyW(m_szLoadingText, TEXT("Shader_VtxAnimMesh Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC,
				TEXT("Shader_VtxAnimMesh"),
				CShader::Create
				(
					m_pDevice,
					m_pContext,
					TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"),
					VTXANIMMESH::Elements,
					VTXANIMMESH::iNumElements
				)
			)))
			return E_FAIL;

		// Shader_Cube
		lstrcpyW(m_szLoadingText, TEXT("Shader_VtxCube Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC,
				TEXT("Shader_VtxCube"),
				CShader::Create
				(
					m_pDevice, 
					m_pContext, 
					TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), 
					VTXCUBE::Elements, 
					VTXCUBE::iNumElements
				)
			)))
			return E_FAIL;

		// Shader_VtxNorTex
		lstrcpyW(m_szLoadingText, TEXT("Shader_VtxNorTex Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC, 
				TEXT("Shader_VtxNorTex"),
				CShader::Create
				(
					m_pDevice, 
					m_pContext, 
					TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), 
					VTXNORTEX::Elements, 
					VTXNORTEX::iNumElements
				)
			)))
			return E_FAIL;
	}

	// VIBuffer
	{
		// VIBuffer_Cube
		lstrcpyW(m_szLoadingText, TEXT("VIBuffer_Cube Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC, 
				TEXT("VIBuffer_Cube"),
				CVIBuffer_Cube::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;

		// VIBuffer_Terrain
		lstrcpyW(m_szLoadingText, TEXT("VIBuffer_Terrain Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC, 
				TEXT("VIBuffer_Terrain"),
				CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp"))
			)))
			return E_FAIL;
	}

	// Texture
	{
		// Texture_Sky
		lstrcpyW(m_szLoadingText, TEXT("Texture_Sky Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC, 
				TEXT("Texture_Sky"),
				CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/SkyBox/Sky_%d.dds", 6)
			)))
			return E_FAIL;

		// Texture_Terrain
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC, 
				TEXT("Texture_Terrain"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2)
			)))
			return E_FAIL;

		// Texture_Mask
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC, 
				TEXT("Texture_Mask"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"))
			)))
			return E_FAIL;

		// Texture_Brush
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_STATIC, 
				TEXT("Texture_Brush"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"))
			)))
			return E_FAIL;
	}

	// Collider
	{
		// Collider_AABB
		lstrcpyW(m_szLoadingText, TEXT("Collider_AABB Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVEL_STATIC,
				TEXT("Collider_AABB"),
				CCollider_AABB::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;

		// Collider_OBB
		lstrcpyW(m_szLoadingText, TEXT("Collider_OBB Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVEL_STATIC,
				TEXT("Collider_OBB"),
				CCollider_OBB::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;

		// Collider_Sphere
		lstrcpyW(m_szLoadingText, TEXT("Collider_Sphere Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVEL_STATIC,
				TEXT("Collider_Sphere"),
				CCollider_Sphere::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;
	}

	// Model
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();

		// Model_Player
		lstrcpyW(m_szLoadingText, TEXT("Model_Player Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Player"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Player/Player/Player.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Player"));

		// Model_PlayerTemp
		lstrcpyW(m_szLoadingText, TEXT("Model_PlayerTemp Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_PlayerTemp"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Player/PlayerTemp/PlayerTemp.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_PlayerTemp"));

		// Model_Boss_Inkerton
		lstrcpyW(m_szLoadingText, TEXT("Model_Boss_Inkerton Loading"));
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Boss_Inkerton"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Boss_Inkerton/Boss_Inkerton.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Boss_Inkerton"));

		// Model_Boss_Roland
		lstrcpyW(m_szLoadingText, TEXT("Model_Boss_Roland Loading"));
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Boss_Roland"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Boss_Roland/Boss_Roland.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Boss_Roland"));

		// Model_Boss_Petroch
		lstrcpyW(m_szLoadingText, TEXT("Model_Boss_Petroch Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Boss_Petroch"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Boss_Petroch/Boss_Petroch.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Boss_Petroch"));

		// Model_Boss_Pagurus
		lstrcpyW(m_szLoadingText, TEXT("Model_Boss_Pagurus Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Boss_Pagurus"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Boss_Pagurus/Boss_Pagurus.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Boss_Pagurus"));


		// Model_Enemy_ShellSpliter
		lstrcpyW(m_szLoadingText, TEXT("Model_Boss_Shell Spliter Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Enemy_ShellSpliter"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Enemy_Splitter/Enemy_Splitter.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Enemy_ShellSpliter"));

		// Model_Enemy_Bowman
		lstrcpyW(m_szLoadingText, TEXT("Model_Enemy_Bowman Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Enemy_Bowman"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Enemy_Bowman/Enemy_Bowman.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Enemy_Bowman"));

		// Model_Enemy_Normie
		lstrcpyW(m_szLoadingText, TEXT("Model_Enemy_Normie Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Enemy_Normie"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Enemy_Normie/Enemy_Normie.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				) 
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Enemy_Normie"));

		// Model_Enemy_Stunpounder
		lstrcpyW(m_szLoadingText, TEXT("Model_Enemy_Stunpounder Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Enemy_Stunpounder"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Enemy_Stunpounder/Enemy_Stunpounder.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Enemy_Stunpounder"));

		// Model_Enemy_Gumpounder
		lstrcpyW(m_szLoadingText, TEXT("Model_Enemy_Gumpounder Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Enemy_Gumpounder"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Enemy_Gumpounder/Enemy_Gumpounder.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Enemy_Gumpounder"));

		// Model_Enemy_Sardine
		lstrcpyW(m_szLoadingText, TEXT("Model_Enemy_Sardine Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Enemy_Sardine"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Enemy_Sardine/Enemy_Sardine.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Enemy_Sardine"));

		// Model_Enemy_BobbitWorm
		lstrcpyW(m_szLoadingText, TEXT("Model_Enemy_BobbitWorm Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(-70.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Enemy_BobbitWorm"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Enemy_BobbitWorm/Enemy_BobbitWorm.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Enemy_BobbitWorm"));

		// Model_Enemy_Cuttlefish
		lstrcpyW(m_szLoadingText, TEXT("Model_Enemy_Cuttlefish Loading"));
		PreTransformMatrix = XMMatrixScaling(0.00006f, 0.00006f, 0.00006f) * XMMatrixRotationY(XMConvertToRadians(155.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Enemy_Cuttlefish"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/Enemy/Enemy_Cuttlefish/Enemy_Cuttlefish.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix * XMMatrixRotationRollPitchYaw(0.f, 157.f, 0.f)
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Enemy_Cuttlefish"));

		// Model_Skill_Topoda
		lstrcpyW(m_szLoadingText, TEXT("Model_Skill_Topoda Loading"));
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Skill_Topoda"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Skill/Topoda/Topoda.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_Skill_Topoda"));

		// Model_NPC_Shark
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_Shark Loading"));
		PreTransformMatrix = XMMatrixScaling(0.0000003f, 0.0000003f, 0.0000003f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_Shark"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/Shark/Shark.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_Shark"));

		// Model_NPC_Roland
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_Roland_Death Loading"));
		PreTransformMatrix = XMMatrixScaling(0.000045f, 0.000045f, 0.000045f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_Roland"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/Roland/Roland.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_Roland"));

		// Model_NPC_Roland_Death
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_Roland_Death Loading"));
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_Roland_Death"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/Roland_Death/Roland.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_Roland_Death"));

		// Model_NPC_ShellSplitter
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_ShellSplitter Loading"));
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_ShellSplitter"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/ShellSplitter/ShellSplitter.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_ShellSplitter"));

		// Model_NPC_Inkerton_Fight
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_Inkerton_Fight Loading"));
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_Inkerton_Fight"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/Inkerton_Fight/Inkerton_Fight.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_Inkerton_Fight"));

		// Model_NPC_Chitan_Fight
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_Chitan_Fight Loading"));
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_Chitan_Fight"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/Chitan_Fight/Chitan_Fight.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_Chitan_Fight"));

		// Model_NPC_BlackSmith
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_BlackSmith Loading"));
		PreTransformMatrix = XMMatrixScaling(0.0000003f, 0.0000003f, 0.0000003f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_BlackSmith"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/BlackSmith/BlackSmith.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_BlackSmith"));

		// Model_NPC_DanceCrab
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_DanceCrab Loading"));
		PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_DanceCrab"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/DanceCrab/DanceCrab.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_DanceCrab"));

		// Model_NPC_Inkerton
		lstrcpyW(m_szLoadingText, TEXT("Model_NPC_Inkerton Loading"));
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_NPC_Inkerton"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Character/NPC/Inkerton/Inkerton.fbx",
					CModel::TYPE::ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_AnimObjectTag(TEXT("Model_NPC_Inkerton"));

	}
	// PartObject
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();

		// Model_Fork
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * 
			XMMatrixRotationY(XMConvertToRadians(180.f)) /** 
			XMMatrixRotationX(XMConvertToRadians(90.f))*/;
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Fork"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Weapon/Fork/Fork.fbx",
					CModel::TYPE::NON_ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Fork"));

		// Model_MiddleFork
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_MiddleFork"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Weapon/MiddleFork/MiddleFork.fbx",
					CModel::TYPE::PRE_TRANSFORM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_MiddleFork"));

		// Model_GoldFork
		PreTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_GoldFork"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Weapon/GoldFork/GoldFork.fbx",
					CModel::TYPE::NON_ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_GoldFork"));

		// Model_Shell_MirrorBall
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Shell_MirrorBall"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Shell/MirrorBall/MirrorBall.fbx",
					CModel::TYPE::NON_ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Shell_MirrorBall"));

		// Model_Shell_MirrorBall_Roll
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, -0.3f, 0.f);
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Shell_MirrorBall_Roll"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Shell/MirrorBall/MirrorBall.fbx",
					CModel::TYPE::NON_ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Shell_MirrorBall_Roll"));

		// Model_Shell_TennisBall
		PreTransformMatrix = XMMatrixScaling(0.0025f, 0.0025f, 0.0025f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Shell_TennisBall"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Shell/TennisBall/TennisBall.fbx",
					CModel::TYPE::PRE_TRANSFORM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Shell_TennisBall"));

		// Model_Shell_TennisBall_Roll
		PreTransformMatrix = XMMatrixScaling(0.0025f, 0.0025f, 0.0025f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, -0.35f, 0.f);
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Shell_TennisBall_Roll"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Shell/TennisBall/TennisBall.fbx",
					CModel::TYPE::PRE_TRANSFORM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Shell_TennisBall_Roll"));

		// Model_Shell_SodaCan
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Shell_SodaCan"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Shell/SodaCan/SodaCan.fbx",
					CModel::TYPE::NON_ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Shell_SodaCan"));

		// Model_Shell_Duck
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Shell_Duck"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Shell/Duck/Duck.fbx",
					CModel::TYPE::PRE_TRANSFORM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Shell_Duck"));

		// Model_Shell_Home
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Shell_Home"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Shell/Home/Home.fbx",
					CModel::TYPE::PRE_TRANSFORM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Shell_Home"));

		// Model_Roland_Wrench
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Roland_Wrench"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Weapon/Wrench/Roland_Wrench_0_R.fbx",
					CModel::TYPE::NON_ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;
		
		IMGUI->Add_PartObjectTag(TEXT("Model_Roland_Wrench"));

		// Model_Petroch_Shell
		PreTransformMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Petroch_Shell"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Equipment/Shell/Petroch_Shell/Petroch_Shell.fbx",
					CModel::TYPE::NON_ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Petroch_Shell"));

		// Model_Item_HeartKelp
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Item_HeartKelp"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Item/HeartKelp/HeartKelp.fbx",
					CModel::TYPE::NON_ANIM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Item_HeartKelp"));

		// Model_Skill_Urchin
		lstrcpyW(m_szLoadingText, TEXT("Model_Skill_Urchin Loading"));
		PreTransformMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("Model_Skill_Urchin"),
				CModel::Create
				(
					m_pDevice,
					m_pContext,
					"../Bin/Resources/Models/Skill/Urchin/Urchin.fbx",
					CModel::TYPE::PRE_TRANSFORM,
					PreTransformMatrix
				)
			)))
			return E_FAIL;

		IMGUI->Add_PartObjectTag(TEXT("Model_Skill_Urchin"));

		// GameObject_CameraDebug_Tool
		lstrcpyW(m_szLoadingText, TEXT("GameObject_CameraDebug_Tool Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("GameObject_CameraDebug_Tool"),
				CCameraDebug_Tool::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;
	}

	// GameObject
	{
		// GameObject_Sky
		lstrcpyW(m_szLoadingText, TEXT("GameObject_Sky Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("GameObject_Sky"),
				CSky::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;

		// GameObject_Terrain
		lstrcpyW(m_szLoadingText, TEXT("GameObject_Terrain Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("GameObject_Terrain"),
				CTerrain::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;

		// GameObject_AnimObject
		lstrcpyW(m_szLoadingText, TEXT("GameObject_AnimObject Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("GameObject_AnimObject"),
				CAnimObject::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;

		// GameObject_PartObject
		lstrcpyW(m_szLoadingText, TEXT("GameObject_PartObject Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("GameObject_PartObject"),
				CToolPartObject::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;

		// GameObject_ToolCamera
		lstrcpyW(m_szLoadingText, TEXT("GameObject_ToolCamera Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("GameObject_ToolCamera"),
				CToolCamera::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;

		// GameObject_CineCamera_Tool
		lstrcpyW(m_szLoadingText, TEXT("GameObject_CineCamera_Tool Loading"));
		if (FAILED(
			m_pGameInstance->Add_Prototype
			(
				LEVELID::LEVEL_TOOL,
				TEXT("GameObject_CineCamera_Tool"),
				CCineCamera_Tool::Create(m_pDevice, m_pContext)
			)))
			return E_FAIL;
	}

	lstrcpyW(m_szLoadingText, TEXT("Tool Finished"));
	m_bFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID)
{
	CLoader* pInstance = new CLoader(_pDevice, _pContext);

	if (FAILED(pInstance->Init(_eNextLevelID)))
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
	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}