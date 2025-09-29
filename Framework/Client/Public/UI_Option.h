#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CUI_Manager;
class CUI_DynamicButton;
class CUI_DynamicTexture;

class CUI_Option final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		CUIObject* pOwner = { nullptr };
	};

private:
	CUI_Option(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Option(const CUI_Option& Prototype);
	virtual ~CUI_Option() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_Manager* m_pUIManager = { nullptr };
	CUIObject* m_pOwner = { nullptr };

	_bool		m_bOn[5] = {true, true, true, true, true};
	_int		m_iVolum[5] = { 100 };
	CUI_DynamicTexture* m_pUIBar[5] = { nullptr };
	CUI_DynamicTexture* m_pUIDot[5] = { nullptr };

	CUI_DynamicTexture*	m_pVolumeBar[5] = { nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Option* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
