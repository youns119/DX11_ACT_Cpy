#pragma once
#include "Client_Defines.h"
#include "MeshEffect.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
END

BEGIN(Client)

class CEffect_Mesh_ActivateShell final : public CMeshEffect
{
	using super = CMeshEffect;

private:
	CEffect_Mesh_ActivateShell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Mesh_ActivateShell(const CEffect_Mesh_ActivateShell& Prototype);
	virtual ~CEffect_Mesh_ActivateShell() = default;

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
	virtual _bool Call_Trigger_Event(_float fTimeDelta) override;

private:
	_float3 m_vOriginScale{};
	_float3 m_vTargetScale{};

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

	CTexture* m_pTextureCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };
	CTexture* m_pNoiseTextureCom{ nullptr };
	CTexture* m_pEmissiveTextureCom{ nullptr };


public:
	static CEffect_Mesh_ActivateShell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END