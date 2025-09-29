#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

struct FPlayerStat;

class CUI_Compass final : public CUI
{
	using super = CUIObject;

public:
	enum DIRECTION
	{
		DIR_EAST,
		DIR_WEST,
		DIR_SOUTH,
		DIR_NORTH,
		DIR_END
	};

	struct DESC
	{
		CUI::DESC UIDesc{};

		FPlayerStat* pPlayerStat;
	};

private:
	CUI_Compass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Compass(const CUI_Compass& Prototype);
	virtual ~CUI_Compass() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_Broken(_bool bBroken) { m_bBroken = bBroken;}

private:
	const _tchar* m_szDirText[DIR_END] = { L"µ¿", L"¼­", L"³²", L"ºÏ"};
	_float2	m_vTextPosition[DIR_END] = {};

	FPlayerStat* m_pPlayerStat = {};

	_bool		m_bBroken = { false };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Compass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
