#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

struct FPlayerStat;
struct FShellStat;

class CUI_Manager;
class CUI_StatusButton;
class CUI_Description;
class CUI_Button;
class CUI_PlayerShellDesc;
class CUI_DynamicTexture;
class CUI_DynamicButton;

enum class ACTIVESKILL;

class CUI_Status final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		CUIObject* pOwner = { nullptr };

		FPlayerStat* pPlayerStat = { nullptr };
		FShellStat** ppShellStat = { nullptr };
	};

private:
	CUI_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Status(const CUI_Status& Prototype);
	virtual ~CUI_Status() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUIObject* m_pOwner = { nullptr };

	FPlayerStat* m_pPlayerStat = { nullptr };
	FShellStat** m_ppShellStat = { nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CUI_StatusButton* m_pStatusButton[5] = {nullptr};
	CUI_Description* m_pUIDesc = { nullptr };
	CUI_PlayerShellDesc* m_pUIShellDesc = { nullptr };
	CUI_Button* m_pUIShellButton = { nullptr };

	CUI_DynamicTexture* m_pUISkillIcon[3] = { nullptr };
	CUI_DynamicTexture* m_pUISkillBorder[3] = { nullptr };
	CUI_DynamicButton* m_pUISkillButton[3] = { nullptr };

	CUI_DynamicTexture* m_pUIPassiveSkillIcon[4] = { nullptr };
	CUI_DynamicButton* m_pUIPassiveSkillButton[4] = { nullptr };

	vector<ACTIVESKILL> m_vecUnlockedSkill;
	vector<ACTIONSKILL> m_vecUnlockedPassiveSkill;

private:
	_wstring m_strStatIconTextureTag = L"Prototype_Component_Texture_StatIcon";
	_wstring m_strSkillIconTextureTag = L"Prototype_Component_Texture_Skill";
	_wstring m_strPassvieSkillIconTextureTag = L"Prototype_Component_Texture_Technique";

private:
	HRESULT Ready_PartObject();

	void Setting_ShellData();
	void Setting_ActiveSkillData();
	void Setting_PassiveSkillData();

public:
	static CUI_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
