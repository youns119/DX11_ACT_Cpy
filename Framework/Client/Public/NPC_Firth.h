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

class CNPC_Firth final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_ATTACK,
		ANIM_TALK,
		ANIM_FAN,
		ANIM_DANCE,
		ANIM_DEATH,
		ANIM_FLAIR,
		ANIM_SEXY,
		ANIM_SIT,
		ANIM_IDLE,
		ANIM_RASPUTIN,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
	};

private:
	CNPC_Firth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Firth(const CNPC_Firth& Prototype);
	virtual ~CNPC_Firth() = default;

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
	CCineCamera* m_pCineCamera = { nullptr };
	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	_bool	m_bChatRender{ false };
	_bool	m_bTalk = { false };
	_bool	m_bBattled = { false };

	const _int	m_iDialogueFileNum = { 11 };
	_int	m_iCurState = {};

	_int m_iLastDialogueIndex = { -1 };

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CNPC_Firth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END