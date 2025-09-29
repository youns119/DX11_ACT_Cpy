#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCineCamera;
class CEffectObject;
END

BEGIN(Client)

class CUI_Manager;
class CCutScene_Manager;
class CUI_Game;

class CNPC_Player final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_ATTACK,
		ANIM_FAN,
		ANIM_TALK,
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
	CNPC_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Player(const CNPC_Player& Prototype);
	virtual ~CNPC_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	void Show_Dialogue();
	ANIM Get_CurAnim() { return m_eCurDance; }

private:
	void Call_Effects();
	void Disable_Effects();

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CCineCamera* m_pCineCamera = { nullptr };
	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	const _int	m_iDialogueFileNum = { 13 };

	_bool	m_bTalk = { false };
	_bool	m_isRender = { false };

	_bool		m_isCutSceneStart = { false };
	_bool		m_isCutSceneEnd = { false };

	_bool		m_bInputable = { false };

	_int	m_iCurState = {};
	_int	m_iLevel = {};
	_int	m_iRound = {};

	CUI_Game* m_pUIGame = { nullptr };
	ANIM		m_eCurDance = ANIM_DANCE;

	vector<CEffectObject*>	m_Effects;

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CNPC_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END