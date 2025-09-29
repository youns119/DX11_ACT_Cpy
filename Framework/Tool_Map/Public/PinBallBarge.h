#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(MapTool)
class CPinBallBarge final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
	};

private:
	CPinBallBarge(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPinBallBarge(const CPinBallBarge& _Prototype);
	virtual ~CPinBallBarge() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};

	_uint	m_iEmissiveMeshIdx = {};
	_float  m_fAccTimeDelta = {};

	_float2 m_vTexCoord = {};
	_uint	m_iX = {};
	_uint	m_iY = {};

private:
	HRESULT Ready_Components();

public:
	static CPinBallBarge* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};
END