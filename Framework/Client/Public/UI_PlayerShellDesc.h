#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

struct FPlayerStat;
struct FShellStat;

class CUI_DefaultStretch;
class CUI_FixStretchBar;
class CUI_Texture;
class CUI_Line;

class CUI_PlayerShellDesc final : public CUIObject
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
	CUI_PlayerShellDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerShellDesc(const CUI_PlayerShellDesc& Prototype);
	virtual ~CUI_PlayerShellDesc() = default;

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

	CUI_DefaultStretch* m_pUIBar = { nullptr };
	CUI_FixStretchBar* m_pUIDefBar = { nullptr };
	CUI_Texture* m_pUIIcon = { nullptr };
	CUI_Line* m_pUILine = { nullptr };
	CUI_Texture* m_pUIDefIcon = { nullptr };
	CUI_Texture* m_pUIWeightIcon = { nullptr };
	CUI_Texture* m_pUISkillPoint[2] = {nullptr};

	CUIObject* m_pOwner = { nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_PlayerShellDesc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
