#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "SpriteEffect.h"
#include "MeshEffect.h"
#include "MeshInstEffect.h"

BEGIN(Client)
class CLevel_Field final : public CLevel
{
	using super = CLevel;

	enum class THEME
	{
		FIELD,
		AZIT,
		MAGNET,
		PINBALL,
		CRABTOMB,

		MAX,
	};

	enum
	{
		THEME_END = THEME::MAX,
	};

private:
	CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Field() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	array<struct THEME_DESC*, THEME_END> m_Themes {};

private:
	HRESULT Ready_Layer_Environment(const _wstring& strLayerTag);
	HRESULT Ready_Layer_MapObject(const _wstring& strLayerTag);
	HRESULT Ready_Layer_InteractionObject(const _wstring& strLayerTag);
	HRESULT Ready_Layer_MapItem(const _wstring& strLayerTag);
	HRESULT Ready_Layer_MapTrigger(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Projectile(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Shell(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_CinematicCamera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_NPC(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Manager(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	HRESULT Ready_layer_PhysicsTest(const _wstring& strLayerTag);
	HRESULT Ready_Environment();
	HRESULT Ready_Layer_Lights(const _wstring& strLayerTag);
#ifdef _DEBUG
	HRESULT Ready_Layer_Debug(const _wstring& strLayerTag);
	HRESULT Ready_Layer_CameraTool(const _wstring& strLayerTag);
#endif // _DEBUG

	void SetUp_CollisionGroup();
	void ReadFile_MeshEffect(HANDLE hFile, CMeshEffect::DESC& Desc);
	void ReadFile_SpriteEffect(HANDLE hFile, CSpriteEffect::DESC& Desc);
	void ReadFile_MeshInstEffect(HANDLE hFile, CMeshInstEffect::DESC& Desc);


public:
	static CLevel_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
