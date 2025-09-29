#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_StatusButton final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float fStretchFactor = { 0.f };

		const _tchar* strIconFileName = L"";

		_uint* pCount = { nullptr };
		std::function<void()> HoverCallback = {};
	};

private:
	CUI_StatusButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StatusButton(const CUI_StatusButton& Prototype);
	virtual ~CUI_StatusButton() = default;

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

	_float m_fFrame = {};
	_float m_fRatio = {};

private:
	HRESULT Ready_PartObject(DESC* pDesc);

public:
	static CUI_StatusButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
