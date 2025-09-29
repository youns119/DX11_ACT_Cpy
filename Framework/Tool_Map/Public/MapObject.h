#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(MapTool)
// 상호작용 없는 맵 오브젝트들을 모델 바꿔가면서 사용할 클래스임
class CMapObject final : public CGameObject
{
	using super = CGameObject;

public:
	enum PHYSICS_STATE { NONE, CONVEX, TRIANGLE };
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
		_uint				  ePhysicsType = {};
	};

private:
	CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMapObject(const CMapObject& _Prototype);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	_uint Get_PhysicState() { return m_iPhysicsState; }
	_float Get_FrustumRange() { return m_fFrustumRange; }
	_float4 Get_BaseColor() { return m_vBaseColor; }
	virtual HRESULT Render_Shadow() override;

	void Set_PhysicState(_uint iPhysicsState) { m_iPhysicsState = iPhysicsState; }
	void Set_FrustumRange(_float fRange) { m_fFrustumRange = fRange; }
	void Set_BaseColor(_float4 vBaseColor) { m_vBaseColor = vBaseColor; }
private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};

	_float  m_fFrustumRange = {};
	_float4  m_vBaseColor = { 1.f, 1.f, 1.f, 1.f };
	_wstring m_strName = L"";
	_uint	m_iPhysicsState = {};

private:
	HRESULT Ready_Components();

public:
	static CMapObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
