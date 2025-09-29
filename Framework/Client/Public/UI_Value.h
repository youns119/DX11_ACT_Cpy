#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Value final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_uint*	pCount = {nullptr};
	};

private:
	CUI_Value(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Value(const CUI_Value& Prototype);
	virtual ~CUI_Value() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_uint*			m_pCount = { nullptr };

protected:
	void Format_With_Comma();

public:
	static CUI_Value* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
