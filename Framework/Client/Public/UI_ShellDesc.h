#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

struct FShellStat;

class CUI_Manager;

class CUI_DefaultStretch;
class CUI_FixStretchBar;
class CUI_Texture;
class CUI_Line;

class CUI_ShellDesc final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;	

		CGameObject* pShell = { nullptr };
		FShellStat* pShellStat = { nullptr };
	};

private:
	CUI_ShellDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ShellDesc(const CUI_ShellDesc& Prototype);
	virtual ~CUI_ShellDesc() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_Manager* m_pUIManager = { nullptr };

	CUI_DefaultStretch* m_pUIBorder = { nullptr };
	CUI_DefaultStretch* m_pUIBar = { nullptr };
	CUI_FixStretchBar* m_pUIDefBar = { nullptr };
	CUI_Texture* m_pUIIcon = { nullptr };
	CUI_Line* m_pUILine = { nullptr };
	CUI_Texture* m_pUIStar = { nullptr };

	CUI_Texture* m_pUIDefIcon = { nullptr };
	CUI_Texture* m_pUIWeightIcon = { nullptr };

	CUI_Texture* m_pUIStatLine = { nullptr };
	CUI_Texture* m_pUICompareLine = { nullptr };
	CUI_Texture* m_pUIArrow = { nullptr };

	CUI_Texture* m_pUIPoint[4] = {nullptr};
	CUI_Texture* m_pUIPointArrow = {nullptr};

	CGameObject* m_pShell = { nullptr };
	FShellStat* m_pShellStat = { nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_ShellDesc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
