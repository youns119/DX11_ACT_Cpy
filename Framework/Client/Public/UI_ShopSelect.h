#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CUI_DynamicButton;

class CUI_ShopSelect final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_ShopSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ShopSelect(const CUI_ShopSelect& Prototype);
	virtual ~CUI_ShopSelect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_DynamicButton* m_pUITrashButton1 = {nullptr};
	CUI_DynamicButton* m_pUITrashButton2 = {nullptr};
	CUI_DynamicButton* m_pUIExitButton = {nullptr};

	_tchar				m_szText[MAX_PATH] = {};


private:
	HRESULT Ready_PartObject();

public:
	static CUI_ShopSelect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
