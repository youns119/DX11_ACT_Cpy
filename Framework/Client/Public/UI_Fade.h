#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Fade final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

protected:
	CUI_Fade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Fade(const CUI_Fade& Prototype);
	virtual ~CUI_Fade() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	_bool	End_FadeIn() { return m_bFadeInEnd; }
	_bool	End_FadeOut() { return m_bFadeOutEnd; }

	void FadeIn();
	void FadeOut();

	void Set_Speed(_float fFadeSpeed) { m_fFadeSpeed = fFadeSpeed; }
private:
	_float m_fAlpha = {1.f};

	_bool	m_bFadeIn = {};
	_bool	m_bFadeOut = {};

	_bool m_bFadeInEnd = {};
	_bool m_bFadeOutEnd = {};

	_float	m_fFadeSpeed = {1.f};

public:
	static CUI_Fade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
