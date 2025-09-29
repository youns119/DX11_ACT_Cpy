#pragma once

#include "GameObject.h"
#include "Renderer.h"

BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
	using super = CGameObject;
public:
	struct UIOBJECT_DESC
	{
		CGameObject::GAMEOBJECT_DESC	GameObjectDesc;

		_float fX, fY, fZ;
		_float fSizeX, fSizeY;
		//_float fNear, fFar;
	};

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	static void Resize(ID3D11DeviceContext* pContext);

	void Set_RenderGroup(CRenderer::RENDERGROUP eRenderGroup) { m_eRenderGroup = eRenderGroup; }
	CRenderer::RENDERGROUP Get_RenderGroup() const { return m_eRenderGroup; }

protected:
	static _float4x4		s_ViewMatrix;
	static _float4x4		s_ProjMatrix;

	static _float s_fViewportWidth;
	static _float s_fViewportHeight;
	static _float s_fWidthMiddle;
	static _float s_fHeightMiddle;

	CRenderer::RENDERGROUP m_eRenderGroup = { CRenderer::RENDERGROUP::UI };

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
