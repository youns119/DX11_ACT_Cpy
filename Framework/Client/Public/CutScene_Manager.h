#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CCutScene_Manager final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CCutScene_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutScene_Manager(const CCutScene_Manager& Prototype);
	virtual ~CCutScene_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Begin_CutScene(_wstring strCutSceneKey);
	void End_CutScene(_wstring strCutSceneKey);
	void Reset_CutScene(_wstring strCutSceneKey) { m_mapCutScenes[strCutSceneKey] = false; }
	void Delete_CutScene(_wstring strCutSceneKey) { m_mapCutScenes.erase(strCutSceneKey); }

	_bool Is_CutSceneFinished(_wstring strCutSceneKey) { return m_mapCutScenes[strCutSceneKey]; }

	_wstring Get_CurPlaying() { return m_strCurPlayingScene; }

private:
	map<_wstring, _bool> m_mapCutScenes;
	_wstring			m_strCurPlayingScene = L"";

public:
	static CCutScene_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
