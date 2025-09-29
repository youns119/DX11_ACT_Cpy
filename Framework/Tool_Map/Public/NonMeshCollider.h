#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(MapTool)

class CNonMeshCollider final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC GameObjectDesc{};
	};

private:
	CNonMeshCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNonMeshCollider(const CNonMeshCollider& Prototype);
	virtual ~CNonMeshCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	//CCollider* m_pColliderCom = { nullptr };

private:
	//HRESULT Ready_Components();

public:
	static CNonMeshCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
