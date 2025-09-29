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

class CNPC_Tortellini final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_CLAP,
		ANIM_LOOKUP,
		ANIM_SHY,
		ANIM_ITALIAN,
		ANIM_IDLE,
		ANIM_TALK,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
	};

private:
	CNPC_Tortellini(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Tortellini(const CNPC_Tortellini& Prototype);
	virtual ~CNPC_Tortellini() = default;

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

	void Pause_CineCamera();
private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CCollider* m_pColliderCom { nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CCineCamera* m_pCineCamera = { nullptr };

	_bool	m_bChatRender = { false };
	_bool	m_bTalk = { false };

	const _int	m_iDialogueFileNum = { 4 };
	_int	m_iCurState = {};

	_int m_iLastDialogueIndex = {-1};

	_bool m_bInFrustum = { false };

private:
	void Check_InFrustum();
	void Change_Animation();

	void CutScene();

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CNPC_Tortellini* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END