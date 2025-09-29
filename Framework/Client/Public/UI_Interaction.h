#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Interaction final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_tchar szText[MAX_PATH] = {};
	};

protected:
	CUI_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Interaction(const CUI_Interaction& Prototype);
	virtual ~CUI_Interaction() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show();
	void Hide();

private:
	_float m_fFrame = {};
	_int	m_iShowCount = {};
	_float	m_fChangeRatio = {5.f};

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Interaction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
