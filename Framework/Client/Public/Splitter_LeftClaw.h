#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "IParried.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CSplitter_LeftClaw final : public CGameObject, public IParried
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC PartObjectDesc{};

		const _float4x4* pSocketMatrix{ nullptr };

		_float fDamage{ 10 };

		CGameObject* pOwner{};
	};

private:
	CSplitter_LeftClaw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSplitter_LeftClaw(const CSplitter_LeftClaw& Prototype);
	virtual ~CSplitter_LeftClaw() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	void Clear_GrabObject() { m_pGrabObject = nullptr; }
	CCollider* Get_Collider() { return m_pColliderCom; }
	CGameObject* Get_GrabObject() { return m_pGrabObject; }

private:
	HRESULT Ready_Components();

private:
	virtual void Parried() override;

public:
	virtual CGameObject* Get_Parried() override { return m_pOwner; }

private:
	CCollider* m_pColliderCom{ nullptr };

	// 매달릴 본의 매트릭스 ? 
	const _float4x4* m_pSocketMatrix{ nullptr };
	_float4x4 m_pGrabMatrix{};

private:
	_float m_fDamage{ 10 };

private:
	CGameObject* m_pOwner;

private:
	CGameObject* m_pGrabObject{};
	_float3		m_vTranslation{};
	_float3		m_vRotation{};

public:
	static CSplitter_LeftClaw* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
