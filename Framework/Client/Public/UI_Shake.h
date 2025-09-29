#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Shake final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

protected:
	CUI_Shake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Shake(const CUI_Shake& Prototype);
	virtual ~CUI_Shake() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_float	m_fShakeAmount = { 1.f };

	_float	m_fFrame = {};

public:
	static CUI_Shake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
