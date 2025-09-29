#pragma once

#include "ToolAnimation_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CVIBuffer_Terrain;
class CTexture;
class CSahder;

END

BEGIN(ToolAnimation)

class CTerrain final 
	: public CGameObject
{
	using super = CGameObject;

public:
	struct MYTERRAIN_DESC
	{
		CGameObject::GAMEOBJECT_DESC GameObjectDesc;
	};

private:
	enum class TEXTURE_TYPE
	{
		DIFFUSE,
		MASK, 
		BRUSH,

		MAX
	};

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	std::array<CTexture*, (_uint)TEXTURE_TYPE::MAX> m_TextureComs{};
	CShader* m_pShaderCom{};
	CVIBuffer_Terrain* m_pVIBufferCom{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END