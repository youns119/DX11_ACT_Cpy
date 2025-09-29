#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Locked final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_bool*	pLocked = { nullptr };
	};

protected:
	CUI_Locked(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Locked(const CUI_Locked& Prototype);
	virtual ~CUI_Locked() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_bool*			m_pLocked = { nullptr };

public:
	static CUI_Locked* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
