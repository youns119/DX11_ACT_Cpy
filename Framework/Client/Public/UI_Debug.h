#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Debug final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

private:
	CUI_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Debug(const CUI_Debug& Prototype);
	virtual ~CUI_Debug() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

public:
	static CUI_Debug* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
