#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Lock final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_bool*	pLockOn = { nullptr };
	};

protected:
	CUI_Lock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Lock(const CUI_Lock& Prototype);
	virtual ~CUI_Lock() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_bool*			m_pLockOn = { nullptr };

	_float			m_fFrame = { 0.f };

public:
	static CUI_Lock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
