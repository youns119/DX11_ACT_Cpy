#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_SkillEffect final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

private:
	CUI_SkillEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillEffect(const CUI_SkillEffect& Prototype);
	virtual ~CUI_SkillEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show();

private:
	_float m_fFrame = {};

public:
	static CUI_SkillEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
