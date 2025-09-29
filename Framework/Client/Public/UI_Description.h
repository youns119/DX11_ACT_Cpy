#pragma once

#include "Client_Defines.h"
#include "UIObject.h"


BEGIN(Client)

class CUI_Manager;
class CUI_Texture;
class CUI_Line;

class CUI_Description final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
		CUIObject* pOwner = {nullptr};

		const _tchar* szBoardFileName = {};
		const _tchar* szBorderFileName = {};
		const _tchar* szLogoFileName = {};
		const _tchar* szIconFileName = {};
		const _tchar* szLineFileName = {};
	};

private:
	CUI_Description(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Description(const CUI_Description& Prototype);
	virtual ~CUI_Description() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show(_bool bShow);
	void Show(_bool bIconShow, _bool bLineShow);

	void Change_Texture(_wstring strTextureTag, _int iTextureNum);
	void Set_Name(const _tchar* szName);

	void Set_Description(const _tchar* szDescription);

	void Change_IconShaderPass(_uint iPassNum);

private:
	CUIObject* m_pOwner = { nullptr };
	CUI_Manager* m_pUIManager = { nullptr };

	CUI_Texture* m_pUIIcon = { nullptr };
	CUI_Line* m_pUILine = { nullptr };

	vector<_wstring> m_vecText;

private:
	HRESULT Ready_PartObject(DESC* pDesc);

public:
	static CUI_Description* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
