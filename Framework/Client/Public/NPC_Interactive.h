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

class CNPC_Interactive final : public CGameObject
{
	using super = CGameObject;

public:

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
		_wstring strModelTag = L"";
		_float3 vColliderExtents = {};
		_float fChatBubbleHeight = {};

		_uint	iIdleAnimNumber = {};
		_uint	iTalkAnimNumber = {};

		_int	iDialogueFileNum = {};
	};

private:
	CNPC_Interactive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Interactive(const CNPC_Interactive& Prototype);
	virtual ~CNPC_Interactive() = default;

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

	_int	m_iDialogueFileNum = {};
	_uint	m_iIdleAnimNumber = {};
	_uint	m_iTalkAnimNumber = {};

	_bool	m_bTalk = { false };
	_bool	m_bTalked = { false };
	_bool	m_bChatRender = { false };

	_bool m_bInFrustum = { false };

private:
	void Check_InFrustum();
	void Change_Animation();

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components(DESC& Desc);
	HRESULT Ready_PartObject(DESC& Desc);

public:
	static CNPC_Interactive* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END