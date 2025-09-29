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

class CNPC_Roland_Final final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_DAMN,
		ANIM_IDLE,
		ANIM_TALK,
		ANIM_WALK,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
	};

private:
	CNPC_Roland_Final(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Roland_Final(const CNPC_Roland_Final& Prototype);
	virtual ~CNPC_Roland_Final() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CCineCamera* m_pCineCamera = { nullptr };
	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	_bool		m_isRender = { false };

	_float		m_fFrame = { 0.f };

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components();

public:
	static CNPC_Roland_Final* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END