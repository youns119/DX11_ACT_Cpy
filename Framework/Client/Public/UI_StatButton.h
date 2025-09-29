#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Manager;
class CUI_DynamicButton;

class CUI_StatButton final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float fStretchFactor = { 0.f };

		const _tchar* strIconFileName = L"";
		_tchar szIconText[MAX_PATH] = {};
		_uint* pValue = { nullptr };

		std::function<void()> HoverCallback = {};
		_uint* pMicroplastic = { nullptr };
		_uint* pLevelUpCost = { nullptr };
		_uint* pLevelDownCost = { nullptr };
	};

private:
	CUI_StatButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StatButton(const CUI_StatButton& Prototype);
	virtual ~CUI_StatButton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Reset_Value();
	void Set_Value(_uint* pValue) { m_pValue = pValue; }
	_uint Get_Value() { return m_iValue; }

private:
	CUI_Manager* m_pUIManager = { nullptr };

	CUI_DynamicButton* m_pUpButton = { nullptr };
	CUI_DynamicButton* m_pDownButton = { nullptr };

	std::function<void()> m_Callback;
	std::function<void()> m_HoverCallback;

	_float m_fFrame = {};
	_float m_fRatio = {};

	_uint* m_pValue = { nullptr };
	_uint m_iValue = {};

	_uint* m_pMicroplastic = { nullptr };
	_uint* m_pLevelUpCost = { nullptr };
	_uint* m_pLevelDownCost = { nullptr };

private:
	HRESULT Ready_PartObject(DESC* pDesc);

	void Set_UpButton();
	void Set_DownButton();
public:
	static CUI_StatButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
