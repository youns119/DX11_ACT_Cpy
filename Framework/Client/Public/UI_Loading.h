#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Loading final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float fSpeed = { 1.f };

		LEVELID* pNextLevelID = {};
	};

private:
	CUI_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading(const CUI_Loading& Prototype);
	virtual ~CUI_Loading() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_float	m_fFrame = {};
	_float m_fSpeed = {};

	LEVELID* m_pNextLevelID = {};
	_uint	m_iTextureNum = {};

	_bool	m_bGamma = { false };
public:
	static CUI_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
