
#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Instancing_Mesh;
class CModel;
END

BEGIN(MapTool)
class CInstancingBushelTree final : public CGameObject
{
public:
	struct DESC
	{

	};

private:
	CInstancingBushelTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancingBushelTree(const CInstancingBushelTree& Prototype);
	virtual ~CInstancingBushelTree() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Input_InstancingInfo(const _int& iNumInstancing, const _float3& vCenter, const _float3& vRange, const _float2& vSize);
	void Save_InstancingFile(HANDLE hFile);
	void Delete_PrevInstancing(_uint _iNumDeleteInstance);

	_bool Find_PickedInstance_WorldMatrix(const _float3& vPickedPos, _float4x4& OutMatrix);
	_bool Get_PickInstanceMesh_WorldMatrix(_float4x4& OutMatrix);
	_bool Set_PickInstanceMesh_WorldMatrix(_float4x4& InMatrix);
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Instancing_Mesh* m_pVIBufferCom = { nullptr };

public:
	HRESULT Ready_Components();

public:
	static CInstancingBushelTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
