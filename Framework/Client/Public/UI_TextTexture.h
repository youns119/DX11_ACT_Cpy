#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)

class CTexture;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CUI_TextTexture final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_TextTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TextTexture(const CUI_TextTexture& Prototype);
	virtual ~CUI_TextTexture() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show(_bool bShowEffect);

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pMaskTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float m_fFrame = 0.f;
	_float m_fTime = 0.f;
	_float m_fTextAlpha = 1.f;

	_bool m_bShowEffect = { false };

private:
	HRESULT Ready_Components();

public:
	static CUI_TextTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
