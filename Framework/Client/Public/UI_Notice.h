#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Item_Trash.h"

BEGIN(Client)

class CUI_Texture;

class CUI_Notice final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

private:
	CUI_Notice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Notice(const CUI_Notice& Prototype);
	virtual ~CUI_Notice() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show(CItem_Trash* pItem);

private:
	CUI_Texture* m_pUIGainItem = {nullptr};
	_float3 m_vItemLocate = {};

	_uint	m_iTextureNum = {};

	_float m_fFrame = {};
	_float m_fOffsetY = {};

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Notice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
