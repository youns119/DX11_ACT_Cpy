#pragma once

#include "Tool_Define.h"
#include "Level.h"

#include "Terrain.h"
#include "Camera_Free.h"

BEGIN(MapTool)

class CMapLevel_Tool final : public CLevel
{
	using super = CLevel;

private:

	enum MAP_OBJ
	{
		WRITE_NAME, PLAYER,
		ALCHOHOLBUILDING, LARGE_PLATEAU, MOONSHELL, CYLINDER00, CYLINDER01, CYLINDER02, CYLINDER03, NCCITY_SHELFCORAL,
		CORALTREE00, CORALTREE01, CORALTREE02, CORALTREE03, CORALTREE04, CORALTREE05, CORAL_BRANCH,
		TUBECORAL00, TUBECORAL01, SHELFCORAL01, SHELFCORAL02, SHELFCORAL03, SHELFCORAL04, SHELFCORAL05,
		BRIDGE, GIN, WHISKEY, RECEIPTROAD,
		TOPSIDE00, TOPSIDE01, TOPSIDE02, TOPSIDE03, TOPSIDE04, PLANE004, PLANE005, PLANE006,
		
		//Field
		WOODPLATE, METALCHAIR, BUTTERTUBTOP, BUTTERTUB, TRASHCANTOP, TRASHCAN,
		RUSTSEACHAIN, SEACHAIN, DUMPSTER, ANCHOR, SPIRE, LARGEKELPTREE, CLUSTER_LARGE00, CLUSTER_LARGE01, CLUSTER_MEDIUM, CLUSTER_SMALL, PLATFORMYROCK00, PLATFORMYROCK01,
		LARGE_SHARP_CLIFF, DUNEFENCE00, LIFERING, TANK, HOLETANK, TIRE, TRASHPILE00, TRASHPILE01, TRASHPILE02, COOLER, COOLERTOP
	};
private:
	CMapLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMapLevel_Tool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Debug(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Environment(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Interaction_Environment(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Lights();
//Object
	HRESULT Save_Interaction_Object();
	HRESULT Load_Interaction_Object();

	HRESULT Save_MapObject();
	HRESULT Load_MapObject();

	void Sheleport_Save();
	void Sheleport_Load();

	HRESULT Save_NPC(const _wstring& strPrototypeTag);
	HRESULT Load_NPC(const _wstring& strPrototypeTag);

	void Save_MapItem();
	void Load_MapItem();

	void Save_MapShell();
	void Load_MapShell();

	void Save_MapTrigger();
	void Load_MapTrigger();

	void Save_Monster();
	void Load_Monster();
                                                                                                                                                           

	string WStringToString(const _wstring& szName);
// Navigation
	HRESULT Picking_NavigationPoint();

#pragma region IMGUI
// ImGui
	HRESULT Update_ImGui();
	void Handle_Manipulate(_float4x4& WorldMat);

	_int m_iTriggerSelected = 0; // 선택된 버튼 값
	_int m_iLightSelected = 0; // 선택된 버튼 값
	_int m_iItemSelected = 0;
	_int m_iShellSelected = 0;
	_int m_iMonsterSelected = 0;
	_int m_iSelected = 0; // 선택된 버튼 값
	_int m_iShell_ID = -1;
	_bool m_bPickInstancing = { false };

	_int m_iShellType = {};

	_bool m_bDebugGrid = { false };

	_float4x4 m_CopyWorldMat{};
	_wstring  m_strName{};

private: 
	_tchar							m_szToolText[MAX_PATH]{};
	_char							m_szInteractionbuffer[MAX_PATH]{};
	_char							m_szbuffer[MAX_PATH] = {};
	_char							m_szTriggerbuffer[MAX_PATH] = {};
	_char							m_szItembuffer[MAX_PATH] = {};
	_char							m_szMonsterbuffer[MAX_PATH] = {};
	_char							m_szLightbuffer[MAX_PATH] = {};
	_char							m_szObjectbuffer[MAX_PATH]{};

// Navigation
	CTerrain*						m_pTerrain = { nullptr };
	vector<_int>					m_PointIndices; // 포인트 3개 돌려쓰기임
	_uint							m_iPickingCnt = {};
	_bool							m_bPointPicking = { false };
	_bool							m_bNaviDelete = { false };

// HeightMap
	_float							m_fRadius = {1.f};
	_float							m_fHeightSize = {1.f};
	_float							m_fSigma = { 0.1f };

// Inastancing
	_int							m_iCurInstancingObject = {};

	class CInstancing_SeaWeeds*		m_pSeaWeeds = { nullptr };
	class CInstancing_KelpBig*		m_pKelpBig = { nullptr };
	class CInstancing_SeaGrass*		m_pSeaGrass00 = { nullptr };
	class CInstancing_KelpGround*	m_pKelpGround = { nullptr };

	_bool							m_bInstancing_Mesh = false;
	_bool							m_bSetMapObject = true;

	_int							m_iNumInstancing = {};
	_float3							m_vCenter = {};
	_float3							m_vRange = {};
	_float2							m_vSize = {};

// Camera
	CCamera_Free*					m_pCamera = { nullptr };
	_float							m_fSpeed = {};

//Object
	_int							m_iCurObjectIdx = { 0 };
	_int							m_iCurMapObjectIdx = { 0 };
	_float							m_fRotation = {};
	_uint							m_iPickedID = {};

	_float4							m_vBaseColor = { 1.f, 1.f, 1.f, 1.f };
	_int							m_iCurMeshIdx = { -1 };
	_int							m_iCurMaterialIdx = { -1 };

	_int							m_iPhysicsState = {};
	_float							m_fFrustumRange = {};

	_bool							m_bMonsterAdd = {false};
//ImGuizmo
	_bool							m_bUseImGuizmo = { false };
	_int							m_iGizmoSRTType  = {};
	_int							m_iInstanceGizmoSRTType  = {};

//LIGHT
	_bool		m_bPointLightPicking = { false };
	_float2		m_vCosCutOff{0.f, 0.f};
	_float4		m_vDirection{0.f, 0.f, 0.f, 1.f};
	_float4		m_vPosition{ 0.f, 0.f, 0.f, 1.f };
	_float		m_fRange{};

	_float4		m_vLightDir{};
	_float m_fAccTimeDelta{};
	_float m_fAngle = {};

	_int		m_iLightType = { };
	//_float2		m_vCosCutOff; 스포트ㅏㄹ이트

	_float4		m_vDiffuse{ 0.f, 0.f, 0.f, 1.f };
	_float4		m_vAmbient{ 0.f, 0.f, 0.f, 1.f };
	_float4		m_vSpecular{ 0.f, 0.f, 0.f, 1.f };

	_float3		m_vAttenuation{0.f, 0.f, 0.f};

	vector<_wstring> m_ToolLightList;

	//Shell
	_uint		m_iCountShell = {};


public:
	static CMapLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
