#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Engine)
class CTexture;
class CEffectObject;
END

BEGIN(Client)
class CItem_Trash final : public CItem
{
	using super = CItem;

public:
	enum EffectType {PARTICLE, SPRITE, TYPE_END};
	struct DESC
	{
		CItem::DESC ItemDesc {};
	};

private:
	CItem_Trash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Trash(const CItem_Trash& Prototype);
	virtual ~CItem_Trash() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_uint Get_ItemNum() { return m_iItemNum; }

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	CCollider*			m_pColliderCom = { nullptr };
	CEffectObject*		m_pItemEffect[TYPE_END] = {nullptr};

	_uint				m_iItemNum = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Effects();

public:
	static CItem_Trash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
