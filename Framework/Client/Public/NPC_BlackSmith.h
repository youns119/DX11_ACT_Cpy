#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CUI_Manager;
class CNPC_Manager;

class CUI_ChatBubble;
class CUI_LocationIcon;

class CNPC_BlackSmith final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_TALK,
		ANIM_IDLE,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
	};

private:
	CNPC_BlackSmith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_BlackSmith(const CNPC_BlackSmith& Prototype);
	virtual ~CNPC_BlackSmith() = default;

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

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	CUI_Manager* m_pUIManager = {nullptr};
	CNPC_Manager* m_pNPCManager = { nullptr };

	CUI_ChatBubble* m_pUIChatBubble = {nullptr};
	CUI_LocationIcon* m_pUILocIcon = {nullptr};

	_bool	m_bChatRender = { false };
	_bool	m_bTalk = { false };

	const _int	m_iDialogueFileNum = { 6 };
	_int		m_iCurState = {};

	_bool m_bInFrustum = { false };

private:
	void Check_InFrustum();
	void Change_Animation();

	void Gain_Skill();
	void Upgrade_Fork();

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CNPC_BlackSmith* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END