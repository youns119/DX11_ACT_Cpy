#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

// 파트 오브젝트로 사용

BEGIN(Engine)

class CTexture;
class CShader;
class CVIBuffer_Trail;

END

BEGIN(Client)

class CEffect_Textured_Trail final : public CGameObject
{
	using super = CGameObject;

private:
	CEffect_Textured_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Textured_Trail(const CEffect_Textured_Trail& Prototype);
	virtual ~CEffect_Textured_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Clear_Info();

private:
	CShader*			m_pShaderCom{};
	CTexture*			m_pTextureCom{};
	CTexture*			m_pColorTextureCom{};
	CVIBuffer_Trail*	m_pBufferCom{};

	const _float4x4*	m_pSocketMatrix{};
	_float				m_fWidth{};
	_float4				m_vCamPosition{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Textured_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END