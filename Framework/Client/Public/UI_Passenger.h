#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Passenger final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

private:
	CUI_Passenger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Passenger(const CUI_Passenger& Prototype);
	virtual ~CUI_Passenger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

public:
	static CUI_Passenger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
