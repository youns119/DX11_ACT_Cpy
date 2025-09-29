#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;

END

BEGIN(Client)

class CPlayer;

class CHand_Player final
	: public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC PartObjectDesc{};

		const _float4x4* pSocketMatrix{};

		CPlayer* pOwner{};
	};

private:
	CHand_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHand_Player(const CHand_Player& Prototype);
	virtual ~CHand_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	void Set_Damage(_float fDamage) { m_fDamage = fDamage; }

public :
	void Collider_Toggle(_bool bToggle);

private:
	CCollider* m_pColliderCom{};

private:
	CPlayer* m_pOwner{};

private :
	_float m_fDamage{ 10.f };

private :
	const _float4x4* m_pSocketMatrix{};

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	HRESULT Ready_Components();

public:
	static CHand_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END