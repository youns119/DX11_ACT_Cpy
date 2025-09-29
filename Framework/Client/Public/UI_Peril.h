#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Peril final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_bool* pIsGrab = {nullptr};
		// 이름 뭐로하실건지 모르겠당
		_bool* pIsUnblockable = {nullptr};
	};

private:
	CUI_Peril(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Peril(const CUI_Peril& Prototype);
	virtual ~CUI_Peril() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	_bool* m_pIsGrab = { nullptr };
	// 이름 뭐로하실건지 모르겠당
	_bool* m_pIsUnblockable = { nullptr };

	_float4 m_vRedColor = {};
	_float4 m_vBlueColor = {};

	_float m_fFrame = {};

public:
	static CUI_Peril* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
