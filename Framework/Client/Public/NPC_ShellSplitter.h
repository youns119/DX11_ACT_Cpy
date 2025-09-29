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

class CNPC_ShellSplitter final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_FEAR,
		ANIM_HIT,
		ANIM_IDLE,
		ANIM_RUN,
	};

	enum STATE
	{
		STATE_RUN,
		STATE_HIT,
		STATE_TALK,
		STATE_FEAR,
		STATE_END,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CNPC_ShellSplitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_ShellSplitter(const CNPC_ShellSplitter& Prototype);
	virtual ~CNPC_ShellSplitter() = default;

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

	const _float3		m_vStartPosition = {297.71f, 0.00f, 340.17f};
	const _float3		m_vGoalPosition = {275.68f, 0.00f, 345.08f};

	STATE		m_eCurState = STATE_RUN;

	_bool		m_isRender = { false };
	_bool		m_isCutSceneStart = { false };
	_bool		m_isCutSceneEnd = { false };
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CNPC_ShellSplitter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END