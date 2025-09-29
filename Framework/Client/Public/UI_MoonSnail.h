#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CUI_Manager;

class CUI_MoonSnail final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_MoonSnail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MoonSnail(const CUI_MoonSnail& Prototype);
	virtual ~CUI_MoonSnail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_Data(_tchar* szLocation, _tchar* szName);

private:
	CUI_Manager* m_pUIManager = { nullptr };

	_tchar m_szLocation[MAX_PATH] = {};
	_tchar m_szName[MAX_PATH] = {};

	_float4 m_vLocationColor = { 11.f / 255.f, 8.f / 255.f, 36.f / 255.f, 1.f };
	_float4 m_vNameColor = { 248.f / 255.f, 252.f / 255.f, 98.f / 255.f, 1.f };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_MoonSnail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
