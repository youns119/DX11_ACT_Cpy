#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(MapTool)
class CPinBall_Bumper final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
	};

private:
	CPinBall_Bumper(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPinBall_Bumper(const CPinBall_Bumper& _Prototype);
	virtual ~CPinBall_Bumper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override; 


	virtual void On_Collision(CGameObject* pGameObject, _uint iGroup) override;

private: 
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};
	CCollider* m_pColliderCom = { nullptr };

	_bool	m_bCollision = { false };
	_float  m_fAccTimeDelta = {};
	_float4 m_vPosition = {};
private:
	HRESULT Ready_Components();

public:
	static CPinBall_Bumper* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};
END