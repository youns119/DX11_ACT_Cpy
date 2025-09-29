#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

struct FPlayerStat;
struct FShellStat;

class CUI_DefaultStretch;
class CUI_DynamicTexture;
class CUI_Manager;

class CUI_Inventory;
class CUI_Status;
class CUI_Option;

class CUI_Menu final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		FPlayerStat* pPlayerStat = {nullptr};
		FShellStat** ppShellStat = {nullptr};
	};

protected:
	CUI_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Menu(const CUI_Menu& Prototype);
	virtual ~CUI_Menu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Change_Color(_float4 vColor);
	_int Get_Menu_Num() { return m_iCurMenuNum; }

	void Left_Menu();
	void Right_Menu();

private:
	FPlayerStat* m_pPlayerStat = { nullptr };
	FShellStat** m_ppShellStat = { nullptr };

	CUI_Manager* m_pUIManager = { nullptr };

	CUI_DynamicTexture* m_pUIDot[3] = { nullptr };

	CUI_DefaultStretch* m_pUITopBorder = { nullptr };
	CUI_DefaultStretch* m_pUIBottomBorder = { nullptr };

	CUI_Inventory* m_pUIInventory = { nullptr };
	CUI_Status* m_pUIStatus = { nullptr };
	CUI_Option* m_pUIOption = { nullptr };

private:
	const _tchar* m_szMenuText[4] = { L"상태", L"쓰레기", L"옵션" };

	_int m_iCurMenuNum = {};
	_float4 m_vRedColor = { 241.f / 255.f, 191.f / 255.f, 181.f / 255.f, 1.f };
	_float4 m_vBlueColor = { 233.f / 255.f, 247.f / 255.f, 255.f / 255.f, 1.f };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Menu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
