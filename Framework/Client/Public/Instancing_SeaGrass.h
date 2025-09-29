
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Instancing_Mesh;
class CModel;
END

BEGIN(Client)
class CInstancing_SeaGrass final : public CGameObject
{
public:
	struct DESC
	{

	};

private:
	CInstancing_SeaGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancing_SeaGrass(const CInstancing_SeaGrass& Prototype);
	virtual ~CInstancing_SeaGrass() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Instancing_Mesh* m_pVIBufferCom = { nullptr };

	_vec3	m_vBendFactor = { 0.f, 0.f, 0.f };
	_vec3	m_vBendDir = { 1.f, 0.f, 1.f };
	_bool	m_bWave = false;
	_float  m_fAccTimeDelta = {};
public:
	HRESULT Ready_Components();

public:
	static CInstancing_SeaGrass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END

