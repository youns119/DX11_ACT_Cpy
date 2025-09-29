#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Mouse final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

private:
	CUI_Mouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Mouse(const CUI_Mouse& Prototype);
	virtual ~CUI_Mouse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_int				m_iTextureNum = {};

public:
	static CUI_Mouse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
