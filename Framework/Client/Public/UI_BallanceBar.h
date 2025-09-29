#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_BallanceBar final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float fChangeRatio = {};
		_float	fEndFactor = { 0.f };

		_float* pCur = { nullptr };
		_float* pMax = { nullptr };
	};

protected:
	CUI_BallanceBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BallanceBar(const CUI_BallanceBar& Prototype);
	virtual ~CUI_BallanceBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_float m_fFrame = {};

	_float	m_fChangeRatio = {};
	_float	m_fEndFactor = {};

	_float* m_pCur = { nullptr };
	_float* m_pMax = { nullptr };

public:
	static CUI_BallanceBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
