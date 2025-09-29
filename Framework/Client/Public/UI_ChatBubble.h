#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_ChatBubble final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_bool* pRender{ nullptr };
	};

private:
	CUI_ChatBubble(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ChatBubble(const CUI_ChatBubble& Prototype);
	virtual ~CUI_ChatBubble() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_bool*				m_pRender = { nullptr };

public:
	static CUI_ChatBubble* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
