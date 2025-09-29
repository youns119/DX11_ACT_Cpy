#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "IParried.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;
class CMovement;
class CTexture;
END

BEGIN(Client)

class CPetroch_Shell final
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
	CPetroch_Shell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPetroch_Shell(const CPetroch_Shell& Prototype);
	virtual ~CPetroch_Shell() = default;

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
	void Dessolve(_float fTimeDelta);
	void Reposition_Shell();

public:
	virtual CGameObject* Get_Parried() override { return m_pOwner; }
	CCollider* Get_Collider() { return m_pColliderCom; }

public:	
	void Set_Dissolve(_bool _IsDissolve) { m_bIsDessolve = _IsDissolve; }

protected:
	HRESULT Load_From_File(const _char* szFilePath, CModel* pModel);

private:
	CModel*				m_pModelCom{ nullptr };
	CShader*			m_pShaderCom{ nullptr };
	CCollider*			m_pColliderCom{ nullptr };	
	CMovement*			m_pMovementCom{ nullptr };
	CGameObject*		m_pOwner{ nullptr };
	CTexture*			m_pNoiseTextureCom{ nullptr };

private:
	const _float4x4*	m_pSocketMatrix{ nullptr };
	_float				m_fDamage{ 10.f };

private:
	_float				m_fSacleRate{ 1.f };

private:
	_bool				m_bIsDessolve{ false };
	_bool				m_bIsDamaged{ false };
	_float				m_fSwellAmount{ 0.f };
	_float				m_fDissolveRate{ 3.5f };
	_float				m_fDissolveDarknessRate{ .0f };

private:
	HRESULT Ready_Components();	
	HRESULT Bind_ShaderResources();

public:
	static CPetroch_Shell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
