#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)

class CTexture;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CUI_Credit;

class CUI_MainMenu final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_MainMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MainMenu(const CUI_MainMenu& Prototype);
	virtual ~CUI_MainMenu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CUI_Credit* m_pUICredit;

private:
	HRESULT Ready_PartObject();

public:
	static CUI_MainMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
