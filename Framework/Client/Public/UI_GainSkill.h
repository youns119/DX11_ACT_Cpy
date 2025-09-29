#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CUI_Manager;
class CUI_Texture;

class CUI_GainSkill final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_GainSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_GainSkill(const CUI_GainSkill& Prototype);
	virtual ~CUI_GainSkill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_PassiveSkill(_int iTextureNum, std::function<void()> callback = {});
	void Set_ActiveSkill(_int iTextureNum, std::function<void()> callback = {});

	void Set_SkillText();
private:
	CUI_Manager* m_pUIManager = { nullptr };
	CUI_Texture* m_pUISkillTexture = { nullptr };

	_float m_fButtonCur = {};
	_float m_fButtonMax = {3.0f};

	_tchar	m_szText[MAX_PATH] = L"";

	_bool	m_bIsRewarded = { false };

	_bool	m_bGainActive = { false };
	_int	m_iSkillNum = {};

	std::function<void()> m_Callback = {};

private:
	HRESULT Ready_PartObject();
	vector<_wstring> m_vecText;

public:
	static CUI_GainSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
