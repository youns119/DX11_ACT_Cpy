#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Manager;

class CUI_Text final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

protected:
	CUI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Text(const CUI_Text& Prototype);
	virtual ~CUI_Text() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_Manager* m_pUIManager = { nullptr };

public:
	static CUI_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
