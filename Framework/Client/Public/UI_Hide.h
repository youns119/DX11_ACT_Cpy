#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

struct FPlayerStat;

class CUI_Hide final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		FPlayerStat* pPlayerStat;
	};

private:
	CUI_Hide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Hide(const CUI_Hide& Prototype);
	virtual ~CUI_Hide() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show();
	void Hide();

private:
	FPlayerStat* m_pPlayerStat = { nullptr };

	_float m_fFrame = {};

	_bool	m_bIsPlayerHide = { false };
	_bool	m_bEnd = { false };

public:
	static CUI_Hide* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
