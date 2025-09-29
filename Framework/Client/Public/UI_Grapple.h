#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

struct FPlayerStat;

class CUI_Manager;

class CUI_Grapple final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		FPlayerStat* pPlayerStat;
		CGameObject** ppGrappleTarget;
	};

private:
	CUI_Grapple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Grapple(const CUI_Grapple& Prototype);
	virtual ~CUI_Grapple() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	FPlayerStat*	m_pPlayerStat = { nullptr };
	CGameObject**	m_ppGrappleTarget = { nullptr };

	CUI_Manager*	m_pUIManager = { nullptr };

	_float			m_fFrame = { 0.f };

	_float			m_fRatio = {};
	const _float	m_fLerpSpeed = { 10.f };
	
	_float2			m_vCenter = {};
public:
	static CUI_Grapple* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
