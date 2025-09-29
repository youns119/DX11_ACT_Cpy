#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Speaker final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

private:
	CUI_Speaker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Speaker(const CUI_Speaker& Prototype);
	virtual ~CUI_Speaker() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Change_TextureNumber(_int iTextureNum) { m_iTextureNum = iTextureNum; }
	void Set_Quiet(_bool bQuiet) { m_bQuiet = bQuiet; }

private:
	_bool	m_bQuiet = { false };

	_float	m_fShakeAmount = { 1.f };

	_float	m_fFrame = {};
	_float	m_fScaleRatio = 1.f;

public:
	static CUI_Speaker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
