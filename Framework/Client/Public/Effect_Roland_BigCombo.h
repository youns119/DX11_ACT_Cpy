#pragma once
#include "Client_Defines.h"
#include "MeshEffect.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
END

BEGIN(Client)

class CEffect_Roland_BigCombo final : public CMeshEffect
{
	using super = CMeshEffect;

private:
	CEffect_Roland_BigCombo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Roland_BigCombo(const CEffect_Roland_BigCombo& Prototype);
	virtual ~CEffect_Roland_BigCombo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Revealage() override;
	virtual HRESULT Render_Distortion() override;

	HRESULT Ready_Components(const _wstring& strModelTag, const _wstring& strTextureTag, LEVELID eLevelID);

private:
	_float3 m_vOriginScale{};

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

	CTexture* m_pTextureCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };
	CTexture* m_pNoiseTextureCom{ nullptr };
	CTexture* m_pEmissiveTextureCom{ nullptr };


public:
	static CEffect_Roland_BigCombo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END