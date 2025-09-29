#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_UmamiEffect final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float2 vLocate = {};
		_int	iIndex = {};
		_float* pUmamiPoint = { nullptr };
	};

private:
	CUI_UmamiEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UmamiEffect(const CUI_UmamiEffect& Prototype);
	virtual ~CUI_UmamiEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_int	m_iIndex = {};
	_float* m_pUmamiPoint = { nullptr };

	_uint m_iCurUmamiPoint = {};

	_float m_fFrame = {};

public:
	static CUI_UmamiEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
