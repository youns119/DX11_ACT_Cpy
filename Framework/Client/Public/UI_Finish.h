#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

struct FPlayerStat;

class CUI_Finish final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		FPlayerStat* pPlayerStat;
		CGameObject** ppFinishTarget;
	};

private:
	CUI_Finish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Finish(const CUI_Finish& Prototype);
	virtual ~CUI_Finish() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	FPlayerStat* m_pPlayerStat = { nullptr };
	CGameObject** m_ppFinishTarget = { nullptr };

	_float			m_fFrame = { 0.f };

	_float			m_fRatio = {};
	const _float	m_fLerpSpeed = { 10.f };

	_float2			m_vCenter = {};
public:
	static CUI_Finish* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
