#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CUI_FixStretch;
class CUI_FixStretchBar;
class CUI_Texture;
class CUI_DynamicTexture;
class CUI_Effect;

class CUI_Game final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		_tchar szName[MAX_PATH] = {};
		_bool* pInputable = { nullptr };
	};

private:
	CUI_Game(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Game(const CUI_Game& Prototype);
	virtual ~CUI_Game() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	_bool Get_Success(){ return m_bSuccess; }
	_bool Get_Fail() { return m_bFail; }
	void Set_Direction(_int iLevel);
private:
	CUI_FixStretch* m_pBar = { nullptr };
	CUI_FixStretchBar* m_pBorder = { nullptr };
	CUI_Texture* m_pClock = { nullptr };
	CUI_DynamicTexture* m_pUIKey[13] = {nullptr};
	CUI_Effect* m_pUIKeyEffect[13] = { nullptr };

	_bool* m_pInputable = { nullptr };
	vector<_int> m_vecDirection;

	vector<_int> m_vecIndex = {};
	_int m_iCurIndex = {};

	_float	m_fCurTime = {5.f};
	_float	m_fMaxTime = {5.f};

	_int m_iCurLevel = {};

	_bool m_bSuccess = { false };
	_bool m_bFail = { false };

private:
	const _float4x4* m_pCenterMatrixPtr{ nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Game* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
