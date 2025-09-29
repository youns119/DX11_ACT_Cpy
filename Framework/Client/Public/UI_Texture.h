#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Texture final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

protected:
	CUI_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Texture(const CUI_Texture& Prototype);
	virtual ~CUI_Texture() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

public:
	static CUI_Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
