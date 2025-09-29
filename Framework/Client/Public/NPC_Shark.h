#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CCineCamera;
END

BEGIN(Client)

class CUI_Manager;
class CCutScene_Manager;

class CNPC_Shark final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_APPEAR,
		ANIM_ATTACK,
		ANIM_DISAPPEAR,
		ANIM_HIDE,
		ANIM_IDLE,
		ANIM_KNOCK,
		ANIM_TALK,
	};

	enum STATE
	{
		STATE_SWIM,
		STATE_APPEAR,
		STATE_KNOCK,
		STATE_WAIT,
		STATE_TALK,
		STATE_LOOK,
		STATE_END,
		STATE_BITE,
		STATE_HIDE,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CNPC_Shark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Shark(const CNPC_Shark& Prototype);
	virtual ~CNPC_Shark() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;
	
	void Event_Appear();
	void Event_Knock();
	void Event_Awake();
	void Event_Bite();
	void Event_Spit();

	void Pause_CineCamera();
	void Stop_CineCamera();

	void Update_StartCutScene(_float fTimeDelta);
	void Update_EndCutScene(_float fTimeDelta);

private:
	void Call_Effect_WaterSplash();
	void Call_Effect_ShellSpark();

private:
	CModel*			m_pModelCom{ nullptr };
	CShader*		m_pShaderCom{ nullptr };
	CCollider*		m_pColliderCom{ nullptr };

	CUI_Manager*	m_pUIManager = { nullptr };
	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	CCineCamera*	m_pCineCamera = { nullptr };

	const _float3	m_vStartPosition = {17.27f, 1.1f, 5.20f};
	const _float3	m_vGoalPosition = {24.04f, 0.8f, 11.17f};

	STATE			m_eCurState = STATE_SWIM;

	_bool	m_bPlayerAwaked = { false };

	const _int	m_iDialogueFileNum = { 0 };
	_bool	m_bChatRender = { false };
	_bool	m_bTalk = { false };

	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CNPC_Shark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END