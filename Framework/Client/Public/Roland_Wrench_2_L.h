#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "IParried.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;

END

BEGIN(Client)

class CRoland_Wrench_2_L final: public CGameObject, public IParried
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
	CRoland_Wrench_2_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRoland_Wrench_2_L(const CRoland_Wrench_2_L& Prototype);
	virtual ~CRoland_Wrench_2_L() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

public:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	virtual void Parried() override;

public:
	virtual CGameObject* Get_Parried() override { return m_pOwner; }

public:
	CCollider* Get_Collider() { return m_pColliderCom; }

protected:
	HRESULT Load_From_File(const _char* szFilePath, CModel* pModel);

private:
	CModel*					m_pModelCom{ nullptr };
	CShader*				m_pShaderCom{ nullptr };
	CCollider*				m_pColliderCom{ nullptr };	
	CGameObject*			m_pOwner{ nullptr };

private:
	const _float4x4*		m_pSocketMatrix{ nullptr };		
	_float					m_fDamage{ 10.f };

private:
	HRESULT Ready_Components();	
	HRESULT Bind_ShaderResources();

public:
	static CRoland_Wrench_2_L* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;	
};

END
