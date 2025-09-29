#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "IParried.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;
class CMovement;
class CPhysics_Controller;
END

BEGIN(Client)

class CBowman_Arrow final
	: public CGameObject,
	public IParried
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC PartObjectDesc{};

		const _float4x4* pSocketMatrix{ nullptr };

		const _char* szFilePath{};

		CModel* pModel{};
	
		_float fDamage{ 10.f };

		CGameObject* pOwner;
	};

private:
	CBowman_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBowman_Arrow(const CBowman_Arrow& Prototype);
	virtual ~CBowman_Arrow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

public:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	virtual void Parried() override;

public:
	virtual CGameObject* Get_Parried() override { return this; }

public:
	CCollider* Get_Collider() { return m_pColliderCom; }

protected:
	HRESULT Load_From_File(const _char* szFilePath, CModel* pModel);

public:	
	void Trigger_Arrow(_vector vTargetPos);
	void Reload_Arrow();

private:
	CModel*					m_pModelCom{ nullptr };
	CShader*				m_pShaderCom{ nullptr };
	CCollider*				m_pColliderCom{ nullptr };
	CMovement*				m_pMovementCom{ nullptr };
	CPhysics_Controller*	m_pPhysicsControllerCom{ nullptr };	
	vector<CGameObject*>	m_pArrows;
	CGameObject*			m_pOwner{ nullptr };

private:
	const _float4x4*		m_pSocketMatrix{ nullptr };
	_bool					m_bIsFired{ false };	
	_int					m_iArrowCount{ 1 };
	_int					m_iCurArrow{ 0 };
	_float					m_fDamage{ 10.f };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Arrows();
	HRESULT Bind_ShaderResources();


public:
	static CBowman_Arrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;	
};

END
