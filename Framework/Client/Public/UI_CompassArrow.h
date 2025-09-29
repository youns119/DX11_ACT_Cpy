#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_CompassArrow final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_bool* pBroken = { nullptr };
	};

private:
	CUI_CompassArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CompassArrow(const CUI_CompassArrow& Prototype);
	virtual ~CUI_CompassArrow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_float m_fRotationSpeed = { 3.f };

	_bool* m_pBroken = { nullptr };
public:
	static CUI_CompassArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
