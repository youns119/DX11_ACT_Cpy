#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
//class CShader;
//class CCollider;
END

BEGIN(MapTool)
class CMapTool_Monster final : public CGameObject
{
	using super = CGameObject;
public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CMapTool_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapTool_Monster(const CMapTool_Monster& Prototype);
	virtual ~CMapTool_Monster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Add_Position(_float3 vPos);
	void Add_Matrix(_float4x4 WorldMat);
	void Delete_Position(_uint Idx);
	vector<_float4x4>& Get_Postions(){ return m_Positions; }
	void Set_WorldPosition(_float3 vPos);
	void DebugRender();

private:
	vector<_float4x4> m_Positions;

public:
	static CMapTool_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END