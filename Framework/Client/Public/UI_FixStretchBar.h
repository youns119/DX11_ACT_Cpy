#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_FixStretchBar final : public CUI
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
	CUI_FixStretchBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_FixStretchBar(const CUI_FixStretchBar& Prototype);
	virtual ~CUI_FixStretchBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_BarValue(_float* pCur, _float* pMax);
	void Reset();
	void Change_EndFactor(_float fEndFactor);
		
private:
	_float m_fFrame = {};

	_float m_fStartFactor = { 0.f };
	_float m_fEndFactor = { 0.f };

	_float	m_fChangeRatio = {};
	_float	m_fStretchSpeed = {};

	_bool	m_bMiddle = { false };
	
	_float* m_pCur = { nullptr };
	_float	m_fPrev = { 0.f };

	_float* m_pMax = { nullptr };
	_float	m_fLast = { 0.f };

	_float m_fTime = { 0.f };
	_float m_fTimeLimit = { 0.5f };

	_bool m_bEffectStart = { false };
	_float m_fEffectTime = { 0.5f };

	// Factor가 1일때의 길이
	_float	m_fLengthStandard = { 0.f };
	// Factor를 1 늘릴 길이
	_float	m_fLengthDelta = { 0.f };

public:
	static CUI_FixStretchBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
