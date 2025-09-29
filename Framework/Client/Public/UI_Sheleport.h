#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

#include "ShellHouse.h"

BEGIN(Client)

class CUI_Manager;
class CUI_Texture;
class CUI_DynamicButton;

class CUI_Sheleport final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_Sheleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Sheleport(const CUI_Sheleport& Prototype);
	virtual ~CUI_Sheleport() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Add_Visited(CShellHouse::DATA tData);

private:
	CUI_Manager* m_pUIManager = { nullptr };

	//map<_wstring, map<_int, _wstring>> m_mapExist;
	
	_int m_iLocationCount = {};
	_int m_iMaxNameCount = {};
	
	unordered_map<_wstring, map<_int, CShellHouse::DATA>> m_mapVisited;

	_int m_iCurHoverIndex = {};
	_float4 m_vTextColor = { 15.f / 255.f, 13.f / 255.f, 33.f / 255.f, 1.f };

	vector<CUI_DynamicButton*> m_vecLeftButton;
	vector<CUI_DynamicButton*> m_vecRightButton;

	CUI_Texture* m_pUICover = { nullptr };

private:
	HRESULT Ready_PartObject();
	void Calculate_Count();

public:
	static CUI_Sheleport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
