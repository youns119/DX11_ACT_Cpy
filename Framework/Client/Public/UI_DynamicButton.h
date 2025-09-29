#pragma once

#include "Client_Defines.h"
#include "UI_Button.h"
#include "Inventory.h"

BEGIN(Client)

class CUI_DynamicButton final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float2 vLocate = {};

		_tchar szText[MAX_PATH] = {};

		std::function<void()> Callback;
		std::function<void()> HoverCallback;
	};

private:
	CUI_DynamicButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DynamicButton(const CUI_DynamicButton& Prototype);
	virtual ~CUI_DynamicButton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_Callback(std::function<void()> callback) { m_Callback = callback; }
	void Set_HoverCallback(std::function<void()> callback) { m_HoverCallback = callback; }

private:
	std::function<void()> m_Callback;
	std::function<void()> m_HoverCallback;

	_float		m_fFrame = {};

public:
	static CUI_DynamicButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
