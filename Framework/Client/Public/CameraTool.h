#pragma once

#ifdef _DEBUG

#include "Client_Defines.h"

#include "GameObject.h"

BEGIN(Client)

class CCamera_Free;
class CCineCameraTool;

class CCameraTool final 
	: public CGameObject
{
	using super = CGameObject;

private:
	CCameraTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraTool(const CCameraTool& Prototype);
	virtual ~CCameraTool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCamera_Free* m_pToolCamera{};
	CCineCameraTool* m_pCineCamera{};

private :
	_bool m_bImGui{};
	_bool m_bTyping{};

private:
	_bool m_bDrawTrajectory{};
	_bool m_bCineCamera{};
	_int m_iPathType{};

	_int m_iPrevCineActorIndex{ -1 };

	_int m_iNotifyIndex{ -1 };
	_float m_fNotifyTrackPosition{};

private:
	void ImGui_CameraTool();
	void ImGui_CameraList();
	void ImGui_CameraPlaying();

	void ImGui_Notify();

private:
	HRESULT Save_CameraData(string _strFile);
	HRESULT Load_CameraData(string _strFile);

private:
	HRESULT ImGuiDialogue_Camera();

private:
	HRESULT Ready_PartObject();

public:
	static CCameraTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END

#endif