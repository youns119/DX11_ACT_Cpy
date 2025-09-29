#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Hurt final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float* pCurHp = { nullptr };
		_float* pMaxHp = { nullptr };
	};

private:
	CUI_Hurt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Hurt(const CUI_Hurt& Prototype);
	virtual ~CUI_Hurt() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_float m_fFrame = {};

	_float* m_pCurHp = { nullptr };
	_float* m_pMaxHp = { nullptr };

	_float	m_fLastHp = {};

public:
	static CUI_Hurt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
