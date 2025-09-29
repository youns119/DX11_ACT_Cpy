#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Texture;

class CUI_ShellBubble final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_tchar szText[MAX_PATH] = {};
	};

protected:
	CUI_ShellBubble(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ShellBubble(const CUI_ShellBubble& Prototype);
	virtual ~CUI_ShellBubble() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_Texture* m_pUICtrl = { nullptr };

	_float	m_fChangeRatio = {10.f};

	_float3 m_vCtrlLocate = {};

private:
	HRESULT Ready_PartObject();

public:
	static CUI_ShellBubble* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
