#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "EShell.h"

BEGIN(Client)

struct FPlayerStat;
struct FShellStat;

class CUI_Shell;
class CUI_Stretch;
class CUI_StretchBar;
class CUI_StretchBarEffect;

class CUI_Umami;
class CUI_UmamiEffect;

class CUI_PlayerLeft final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		FPlayerStat* pPlayerStat;
		FShellStat** ppShellStat;
	};

private:
	CUI_PlayerLeft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerLeft(const CUI_PlayerLeft& Prototype);
	virtual ~CUI_PlayerLeft() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	FPlayerStat* m_pPlayerStat = { nullptr };
	FShellStat** m_ppShellStat = { nullptr };

	FShellStat* m_pLastShellStat = { nullptr };
	_bool	m_bHasShell = { false };


	CUI_Shell* m_pUIShell = { nullptr };

	CUI_Stretch* m_pUIHpBorder = { nullptr };
	CUI_StretchBar* m_pUIHpBar = { nullptr };
	CUI_StretchBarEffect* m_pUIHpEffectBar = { nullptr };

	CUI_Stretch* m_pUIShellBorder = { nullptr };
	CUI_StretchBar* m_pUIShellBar = { nullptr };
	CUI_StretchBarEffect* m_pUIShellEffectBar = { nullptr };

	CUI_Umami* m_pUIUmami[5] = { nullptr };
	CUI_UmamiEffect* m_pUIUmamiEffect[5] = { nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_PlayerLeft* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
