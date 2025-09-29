#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Effect final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float2 vLocate = {};
	};

private:
	CUI_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Effect(const CUI_Effect& Prototype);
	virtual ~CUI_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show();
	void Reset();

private:
	_float m_fFrame = {};

public:
	static CUI_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
