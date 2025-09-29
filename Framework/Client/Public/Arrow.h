#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CFSM;
class CMovement;
class CPhysics_Controller;
END

BEGIN(Client)
class CArrow final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
		wstring name;
	};

private:
	CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArrow(const CArrow& Prototype);
	virtual ~CArrow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:	
	void Set_Active(_bool _bisActive) { m_bIsActive = _bisActive; }	
	void Fire_Arrow(_vector vTargetPos, _float4x4 StartMat);

	void Locate(_fvector vTargetPos) { m_pTransformCom->Set_State(CTransform::STATE::POSITION, vTargetPos); }
	void Look(_fvector vTargetPos) { m_pTransformCom->Set_State(CTransform::STATE::LOOK, vTargetPos); }
	_vec3 Get_Locate() { return m_pTransformCom->Get_State(CTransform::STATE::POSITION); }

public:
	_bool Detect_Ground();

public:
	void Active() {};
	void Deactive();

public:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

private:
	CTransform*				m_pPlayerTransform{ nullptr };
	CShader*				m_pShaderCom { nullptr };
	CModel*					m_pModelCom { nullptr };
	CMovement*				m_pMovementCom{ nullptr };
	CCollider*				m_pColliderCom { nullptr };

private:
	_bool					m_bIsActive{ true };
	_float					m_fLifeTime{ 15.f };
	_float					m_fLifeTimeAcc{};

public:
	static CArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
