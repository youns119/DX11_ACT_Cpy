#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_DynamicTexture final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float2 vLocate = {};
	};

private:
	CUI_DynamicTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DynamicTexture(const CUI_DynamicTexture& Prototype);
	virtual ~CUI_DynamicTexture() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

public:
	static CUI_DynamicTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
