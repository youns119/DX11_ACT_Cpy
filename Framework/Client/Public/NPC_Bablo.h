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

class CNPC_Bablo final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_IDLE,
		ANIM_TALK,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
	};

private:
	CNPC_Bablo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Bablo(const CNPC_Bablo& Prototype);
	virtual ~CNPC_Bablo() = default;

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
	CCollider* m_pColliderCom { nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CNPC_Manager* m_pNPCManager = { nullptr };

	_bool	m_bChatRender = { false };
	_bool	m_bTalk = { false };

	const _int	m_iDialogueFileNum = { 1 };
	_int		m_iCurState = {};

	_bool m_bInFrustum = { false };

private:
	void Check_InFrustum();
	void Change_Animation();

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();

public:
	static CNPC_Bablo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END