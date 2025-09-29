#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)

class CTexture;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CUI_CopyTexture final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_CopyTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CopyTexture(const CUI_CopyTexture& Prototype);
	virtual ~CUI_CopyTexture() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Show();
	void Hide();

	void Set_ShowCallback(std::function<void()> callback) { m_ShowCallback = callback; }
	void Set_HideCallback(std::function<void()> callback) { m_HideCallback = callback; }

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pMaskTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float m_fFrame = 0.f;
	_float m_fInterval = 0.1f;
	_float m_fThreshold = 0.f;

	_bool m_bShow = { false };

	std::function<void()> m_ShowCallback;
	std::function<void()> m_HideCallback;
private:
	HRESULT Ready_Components();

public:
	static CUI_CopyTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END
