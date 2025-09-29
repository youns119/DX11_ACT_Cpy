#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_BallanceBorder final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float fStretchFactor = { 0.f };
		_bool	bMiddle = { false };
	};

protected:
	CUI_BallanceBorder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BallanceBorder(const CUI_BallanceBorder& Prototype);
	virtual ~CUI_BallanceBorder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Blink();
	void Disappear();

private:
	_float m_fFrame = {};

	_bool	m_bMiddle = { false };
	_bool	m_bDisappear{ false };

public:
	static CUI_BallanceBorder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
