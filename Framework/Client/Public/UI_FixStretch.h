#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_FixStretch final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float fStartFactor = { 0.f };
		_float fEndFactor = { 0.f };
		
		_float fChangeRatio = { 1.f };
		_float fStretchSpeed = { 1.f };

		_bool	bMiddle = { false };

		_float* pCur = {nullptr};
		_float* pMax = {nullptr};
	};

protected:
	CUI_FixStretch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_FixStretch(const CUI_FixStretch& Prototype);
	virtual ~CUI_FixStretch() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Reset();
	_bool End_Stretch() { return m_bStretched; }
	void Change_EndFactor(_float fEndFactor);

private:
	_float m_fFrame = {};

	_float m_fStartFactor = { 0.f };
	_float m_fEndFactor = { 0.f };

	_float	m_fChangeRatio = {};
	_float	m_fStretchSpeed = {};

	_bool	m_bMiddle = { false };
	
	_float* m_pCur = { nullptr };
	_float* m_pMax = { nullptr };

	_bool	m_bStretched = { false };

public:
	static CUI_FixStretch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
