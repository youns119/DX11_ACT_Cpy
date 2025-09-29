#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer;
END

BEGIN(Client)
/* 우선 만들고 형님한테 물어봐서 이걸 이펙트로 편입시킬 수 있으면 그렇게 하자 */
class CWaterShadow final : public CGameObject
{
	using super = CGameObject;
public:
	struct CAUSTICS_PARAMS
	{
		_vec2 vSpeed { 0.15f, 0.08f };
		_float fTime { 0.f };
		_float fDistortionFactor { 0.05f };
		_float fIntensity { 1.f };
	};

	struct DESC
	{
		GAMEOBJECT_DESC GameObjectDesc {};
		CAUSTICS_PARAMS CauticsParams {};
	};

private:
	CWaterShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterShadow(const CWaterShadow& Prototype);
	virtual ~CWaterShadow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer*			m_pVIBufferCom { nullptr };
	CShader*			m_pShaderCom { nullptr };
	CTexture*			m_pTextureCom { nullptr };
	CTexture*			m_pTextureCom_Distortion { nullptr };

	CAUSTICS_PARAMS	m_CausticsParams {};

private:
	HRESULT Ready_Component();

public:
	static CWaterShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
