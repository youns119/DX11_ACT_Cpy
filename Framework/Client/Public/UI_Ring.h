#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Manager;

class CUI_Ring final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_bool* pInAir = { nullptr };
		_bool* pGrappling = { nullptr };
		_float* pSwimmingTimeAcc = { nullptr };
		_float* pSwimmingMaxTime = { nullptr };
	};

private:
	CUI_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Ring(const CUI_Ring& Prototype);
	virtual ~CUI_Ring() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_Manager* m_pUIManager = { nullptr };

	_float			m_fFrame = { 0.f };

	_bool*			m_pInAir = { nullptr };
	_bool*			m_pGrappling = { nullptr };
	_float*			m_pSwimmingTimeAcc = { nullptr };
	_float*			m_pSwimmingMaxTime = { nullptr };

	_float			m_fRatio = {};
public:
	static CUI_Ring* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
