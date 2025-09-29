#pragma once

#include "Client_Defines.h"
#include "SpriteEffect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CEffect_Sprite_Once final : public CSpriteEffect
{
	using super = CSpriteEffect;

private:
	CEffect_Sprite_Once(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Sprite_Once(const CEffect_Sprite_Once& Prototype);
	virtual ~CEffect_Sprite_Once() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Revealage() override;

	virtual void Clear_Info(_float fTimeDelta) override;

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Particle_Point* m_pVIBufferCom{ nullptr };

	CTexture* m_pTextureCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };
	CTexture* m_pNoiseTextureCom{ nullptr };
	CTexture* m_pEmissiveTextureCom{ nullptr };


public:
	HRESULT Ready_Components(const _wstring& strTextureTag, LEVELID eLevelID, CVIBuffer_Particle_Point::DESC& Desc);

public:
	static CEffect_Sprite_Once* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END