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

class CNPC_Prawnathan final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
	};

private:
	CNPC_Prawnathan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Prawnathan(const CNPC_Prawnathan& Prototype);
	virtual ~CNPC_Prawnathan() = default;

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

	HRESULT Ready_Components();
	HRESULT Ready_PartObject();

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CCollider* m_pColliderCom { nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CNPC_Manager* m_pNPCManager = { nullptr };

	CUI_ChatBubble* m_pUIChatBubble = { nullptr };
	CUI_LocationIcon* m_pUILocIcon = { nullptr };

	_bool	m_bChatRender = { false };
	_bool	m_bShopRender = { false };

	const _int	m_iDialogueFileNum = { 9 };
	_int	m_iCurState = {};

private:
	HRESULT Load_Data(DESC& Desc);

public:
	static CNPC_Prawnathan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END