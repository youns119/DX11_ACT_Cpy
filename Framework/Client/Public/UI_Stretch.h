#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Stretch final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		_float fChangeRatio = { 1.f };
		_float fStartFactor = { 0.f };

		_bool	bMiddle = { false };

		_float	fLengthStandard = {100.f};
		_float	fLengthDelta = {10.f};

		_float* pCur = {nullptr};
		_float* pMax = {nullptr};

		// 무슨 값을 기준으로 늘어날지..
		_float* pTemp = { nullptr };
	};

protected:
	CUI_Stretch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Stretch(const CUI_Stretch& Prototype);
	virtual ~CUI_Stretch() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_BarValue(_float* pCur, _float* pMax, _float* pTemp = nullptr);

private:
	_float	m_fStretchFactor = { 1.0f };
	_float m_fStartFactor = { 0.f };

	_float	m_fChangeRatio = {};
	_bool	m_bMiddle = { false };
	
	_float* m_pCur = { nullptr };
	_float* m_pMax = { nullptr };
	_float* m_pTemp = { nullptr };

	_float	m_fLengthStandard = { 0.f };
	_float	m_fLengthDelta = { 0.f };

public:
	static CUI_Stretch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
