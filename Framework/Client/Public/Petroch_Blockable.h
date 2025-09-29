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

class CPetroch_Blockable final
	: public CGameObject
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
	CPetroch_Blockable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPetroch_Blockable(const CPetroch_Blockable& Prototype);
	virtual ~CPetroch_Blockable() = default;

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
	void Reposition_Shell();

private:
	void Call_BlockEffect(CGameObject* pOther);

public:
	CCollider* Get_Collider() { return m_pColliderCom; }

protected:
	HRESULT Load_From_File(const _char* szFilePath, CModel* pModel);

private:
	CCollider* m_pColliderCom{ nullptr };
	CGameObject* m_pOwner{ nullptr };

private:
	const _float4x4* m_pSocketMatrix{ nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPetroch_Blockable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
