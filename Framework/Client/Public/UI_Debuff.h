#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

struct FPlayerStat;

class CUI_Debuff final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		FPlayerStat* pPlayerStat;
	};

private:
	CUI_Debuff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Debuff(const CUI_Debuff& Prototype);
	virtual ~CUI_Debuff() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	FPlayerStat* m_pPlayerStat = { nullptr };

	_float			m_fMaxFearGauge = {};
private:
	HRESULT Ready_PartObject();

public:
	static CUI_Debuff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
