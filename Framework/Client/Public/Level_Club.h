#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "SpriteEffect.h"
#include "MeshEffect.h"

BEGIN(Client)

class CLevel_Club final : public CLevel
{
	using super = CLevel;

	struct SPOTLIGHT {
		_wstring strName;

		_float4 vPosition; 
		_float4 vColor;
		_float fRotationSpeed;
		_float fCurrentAngle;
		_float fVerticalAngle;
	};
private:
	CLevel_Club(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Club() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Environment(const _wstring& strLayerTag);
	HRESULT Ready_Environment();
	HRESULT Ready_Layer_MapObject(const _wstring& strLayerTag);
	HRESULT Ready_Layer_InteractionObject(const _wstring& strLayerTag);
	HRESULT Ready_Layer_MapTrigger(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Projectile(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_CinematicCamera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_NPC(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Manager(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	//HRESULT Ready_Lights();

#ifdef _DEBUG
	HRESULT Ready_Layer_Debug(const _wstring& strLayerTag);
	HRESULT Ready_Layer_CameraTool(const _wstring& strLayerTag);
#endif // _DEBUG

	void SetUp_CollisionGroup();
	void ReadFile_MeshEffect(HANDLE hFile, CMeshEffect::DESC& Desc);
	void ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc);

	_float m_fAccTimeDelta = {};
	_float4 m_vLightDir = {};

	vector<SPOTLIGHT> m_ToolLightList;

public:
	static CLevel_Club* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END