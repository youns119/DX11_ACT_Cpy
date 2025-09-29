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

class CNPC_Children final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_IDLE,
		ANIM_TALK,
		ANIM_RUN,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		_wstring strModelTag = L"";
		_float fAngleOffset = {};
	};

private:
	CNPC_Children(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Children(const CNPC_Children& Prototype);
	virtual ~CNPC_Children() = default;

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
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CNPC_Manager* m_pNPCManager = { nullptr };

	CUI_ChatBubble* m_pUIChatBubble = { nullptr };
	CUI_LocationIcon* m_pUILocIcon = { nullptr };

	_bool	m_bChatRender = { false };

	const _int	m_iDialogueFileNum = { 2 };
	static _int		s_iCurState;

	static const _float3 s_vCenter;
	static _bool s_bTalk;
	static _float s_fTime;

	_float m_fAngleOffset = {};
	_float m_fRotationSpeed = { 0.3f };

	const _float s_fRadius = {7.f};

	_bool m_bInFrustum = { false };

private:
	void Check_InFrustum();
	void Talk();
	void Move(_float fTimeDelta);

private:
	HRESULT Ready_Components(DESC* pDesc);
	HRESULT Ready_PartObject();

public:
	static CNPC_Children* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END