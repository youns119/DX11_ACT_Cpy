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
class CCutScene_Manager;

class CNPC_GuardCrab final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_CLAP,
		ANIM_DANCE,
		ANIM_JUMP,
		ANIM_ROAR,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
	};

private:
	CNPC_GuardCrab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_GuardCrab(const CNPC_GuardCrab& Prototype);
	virtual ~CNPC_GuardCrab() = default;

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
	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	_bool	m_bTalk = { false };
	const _int	m_iDialogueFileNum = { 12 };

	_bool m_bAnimStop = {false};
	_uint m_iDanceIndex = {};

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components();

public:
	static CNPC_GuardCrab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END