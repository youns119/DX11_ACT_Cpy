#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Button : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_tchar szText[MAX_PATH] = {};

		std::function<void()> HoverCallback;
		std::function<void()> ClickCallback;
	};

protected:
	CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Button(const CUI_Button& Prototype);
	virtual ~CUI_Button() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_HoverCallback(std::function<void()> callback) { m_HoverCallback = callback; }
	void Set_ClickCallback(std::function<void()> callback) { m_ClickCallback = callback; }

private:
	std::function<void()> m_HoverCallback;
	std::function<void()> m_ClickCallback;

	_float m_fFrame = {};

public:
	static CUI_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
