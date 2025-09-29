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

class CNPC_Inkerton_Final final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_IDLE,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
	};

private:
	CNPC_Inkerton_Final(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Inkerton_Final(const CNPC_Inkerton_Final& Prototype);
	virtual ~CNPC_Inkerton_Final() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	void Look();

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	CCineCamera* m_pCineCamera = { nullptr };

	vector<_wstring> m_vecSoundKeys;
	vector<_wstring> m_vecCaptions;
	_int	m_iCurPlaySoundIndex = {};

	_bool	m_bStart = { false };

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CNPC_Inkerton_Final* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END