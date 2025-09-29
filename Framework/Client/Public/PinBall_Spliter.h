#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Interactive_Bounceable.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysics;
END

BEGIN(Client)
class CPinBall_Spliter final : public CGameObject, public IInteractive_Bounceable
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
	};

private:
	CPinBall_Spliter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPinBall_Spliter(const CPinBall_Spliter& _Prototype);
	virtual ~CPinBall_Spliter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void On_CollisionEnter(CGameObject* pGameObject, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pGameObject, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pGameObject, _uint iGroup) override;

public:
	virtual _vector Get_OffsetPosition() override;

private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};
	CCollider* m_pColliderCom = { nullptr };
	CPhysics* m_pPhysicsCom{ nullptr };

	_bool m_bCollision = { false };
	_float m_fAccTimeDelta = {};

private:
	HRESULT Ready_Components();

public:
	static CPinBall_Spliter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};
END