#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CUI_Manager;

class CRemains final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CRemains(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRemains(const CRemains& Prototype);
	virtual ~CRemains() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

	void Drop(_float3 vPos, _uint iCurLevelIndex);

private:
	CCollider* m_pColliderCom = { nullptr };

	CUI_Manager* m_pUIManager = { nullptr };

	_uint m_iMicroplastic = {};
	_int m_iCurLevelIndex = {-1};

	_bool	m_isRender = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();

public:
	static CRemains* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END