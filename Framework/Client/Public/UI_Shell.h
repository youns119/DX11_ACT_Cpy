#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

struct FShellStat;

class CUI_Texture;
class CUI_ShellBubble;

class CUI_Shell final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

private:
	CUI_Shell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Shell(const CUI_Shell& Prototype);
	virtual ~CUI_Shell() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show();
	void Hide();

	void Set_Skill(_wstring strSkillName);
	void Set_ShellDef(_float fShellDef);

private:
	_float m_fFrame = {};

	CUI_Texture* m_pUIShellDef = { nullptr };
	CUI_ShellBubble* m_pUIShellBubble = { nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Shell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
