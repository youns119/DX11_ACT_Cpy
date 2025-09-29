#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Line final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};
		_bool	bMiddle = { false };
	};

protected:
	CUI_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Line(const CUI_Line& Prototype);
	virtual ~CUI_Line() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_float	m_fChangeRatio = {30.f};
	_bool	m_bMiddle = { false };

public:
	static CUI_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
