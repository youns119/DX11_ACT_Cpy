#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCineCamera;
END

BEGIN(Client)

class CUI_Manager;
class CCutScene_Manager;

class CNPC_Fighter final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
		const _tchar* szFightFileName = L"";

		_wstring strModelTag = L"";
		_wstring strFightModelTag = L"";
	};

private:
	CNPC_Fighter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Fighter(const CNPC_Fighter& Prototype);
	virtual ~CNPC_Fighter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	
	void KnockBack_Player();

private:
	void Call_InkertonSquid();

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	_tchar m_szFightFileName[MAX_PATH] = {};
	_wstring m_strFightModelTag = {};

	_bool m_bLoaded = { false };
private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Load_Data();
	HRESULT Ready_Components(DESC* pDesc);
	HRESULT Ready_Event();
	HRESULT Ready_PartObject();

public:
	static CNPC_Fighter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END