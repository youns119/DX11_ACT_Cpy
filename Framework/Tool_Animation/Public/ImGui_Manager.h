#pragma once

#include "ToolAnimation_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CAnimation;

END

BEGIN(ToolAnimation)

class CAnimObject;
class CToolPartObject;
class CToolCamera;
class CCineCamera_Tool;

class CImgui_Manager final
	: public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

public:
	enum class EToolType
	{
		EToolType_Animation,
		EToolType_Camera,

		EToolType_End
	};

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Init(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Pre_Update(_float _fTimeDelta);
	void Update(_float _fTimeDelta);
	void Post_Update(_float _fTimeDelta);
	HRESULT Render();

	// AnimObject
public:
	_bool Get_IsTyping() const { return m_bPrevTyping || m_bPostTyping || m_bPrevEditTyping || m_bPostEditTyping; }

public:
	void Set_AnimObject(CAnimObject* _pAnimObject);
	void Set_ToolCamera(CToolCamera* _pToolCamera);

public:
	void Add_AnimObjectTag(const _wstring& _strAnimObject)
	{
		m_vecAnimObject.push_back(_strAnimObject);
	}

private:
	EToolType m_eToolType{};

private:
	CAnimObject* m_pAnimObject{};
	CAnimation* m_pPrevAnimation{};
	CAnimation* m_pCurrAnimation{};
	CAnimation* m_pPostAnimation{};

private:
	vector<_wstring> m_vecAnimObject;

	_wstring m_strAnimObject{};

private:
	_int m_iPrevAnimIndex{};
	_int m_iCurrAnimIndex{};
	_int m_iPostAnimIndex{};

private:
	_bool m_bPrevAnimPlaying{ true };
	_bool m_bPostAnimPlaying{ false };

private:
	_bool m_bPrevTyping{ false };
	_bool m_bPrevEditTyping{ false };
	_bool m_bPostTyping{ false };
	_bool m_bPostEditTyping{ false };

private:
	_double m_fPrevNotifyTrackPosition{};
	_double m_fPostNotifyTrackPosition{};

	_int m_iPrevNotifyIndex{ -1 };
	_int m_iPostNotifyIndex{ -1 };

	// PartObject
public:
	void Set_PartObject(CToolPartObject* _pPartObject);

public:
	void Add_PartObjectTag(const _wstring& _strPartObject)
	{
		m_vecPartObject.push_back(_strPartObject);
	}

private:
	vector<_wstring> m_vecPartObject;

	_wstring m_strPartObject{};
	const _char* m_szBoneName{};

private:
	CToolPartObject* m_pPartObject{};

private:
	_bool m_bPartObject{};

	_float m_vRotation[3]{};

private:
	CToolCamera* m_pToolCamera{};
	CCineCamera_Tool* m_pCineCamera{};

private:
	_bool m_bDrawTrajectory{};
	_bool m_bCineCamera{};
	_int m_iPathType{};

	_int m_iPrevCineActorIndex{ -1 };

private:
	void ImGui_AnimationTool();
	void ImGui_AnimObject();
	void ImGui_AnimProperty();
	void ImGui_AnimProperty_Prev();
	void ImGui_AnimProperty_Post();
	void ImGui_AnimationPlaying();
	void ImGui_AnimationPlaying_Prev();
	void ImGui_AnimationPlaying_Post();
	void ImGui_AnimationPlayingButton();
	void ImGui_AnimNotify();
	void ImGui_PrevAnimNotify();
	void ImGui_PostAnimNotify();
	void ImGui_AnimSave();

private:
	HRESULT Save_Animation();
	HRESULT Save_Animation_All();

private:
	void ImGui_PartObject();
	void ImGui_PartObject_Property();
	void ImGui_PartObject_Transform();

private:
	HRESULT Save_PartObject(string _strFile);
	HRESULT Load_PartObject(string _strFile);

private:
	HRESULT ImGuiDialogue_PartObject();

private:
	void ImGui_CameraTool();
	void ImGui_CameraList();
	void ImGui_CameraPlaying();

private:
	HRESULT Save_CameraData(string _strFile);
	HRESULT Load_CameraData(string _strFile);

private:
	HRESULT ImGuiDialogue_Camera();

public:
	virtual void Free() override;
};

#define IMGUI CImgui_Manager::GetInstance()

END