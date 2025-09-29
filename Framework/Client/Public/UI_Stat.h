#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CUI_Manager;
class CUI_Description;
class CUI_StatButton;

class CUI_Stat final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Stat(const CUI_Stat& Prototype);
	virtual ~CUI_Stat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_Manager* m_pUIManager = { nullptr };
	CUI_Description* m_pUIDesc = { nullptr };

	CUI_StatButton* m_pStatButton[4] = { nullptr };

	_wstring m_strStatIconTextureTag = L"Prototype_Component_Texture_StatIcon";

	_uint	m_iMicroplastic = {};
	_uint	m_iLevelUpCost = {};
	_uint	m_iLevelDownCost = {};
	_uint	m_iLevel = {};

private:
	HRESULT Ready_PartObject();
	_wstring Format_With_Comma(_float fValue);

public:
	static CUI_Stat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
