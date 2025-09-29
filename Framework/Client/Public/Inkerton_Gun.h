#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "IParried.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CInkerton_Gun final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC PartObjectDesc{};

		const _float4x4* pSocketMatrix{ nullptr };
		CGameObject* pOwner{ nullptr };		
	};

private:
	CInkerton_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInkerton_Gun(const CInkerton_Gun& Prototype);
	virtual ~CInkerton_Gun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Bullets();

public:
	void Fire_Bullet();

private:
	const _float4x4*					m_pSocketMatrix{ nullptr };
	CGameObject*						m_pOwner{ nullptr };
	CCollider*							m_pColliderCom{ nullptr };	

private:
	_int								m_iCurBulletIndex{ 0 };
	_int								m_iBulletCount{ 2 };
	vector<class CInkerton_Bullet*>		m_Bullets{};

public:
	static CInkerton_Gun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;	
};

END
