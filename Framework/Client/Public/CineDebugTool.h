#pragma once

#include "Client_Defines.h"

#include "GameObject.h"

BEGIN(Client)

class CCineDebugTool final
	: public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC GameObjectDesc{};

		_float3 vPosition{};
	};

private:
	CCineDebugTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCineDebugTool(const CCineDebugTool& Prototype);
	virtual ~CCineDebugTool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Position(_fvector vPos);

public:
	void Toggle_Collider(_bool bToggle);

private:
	CCollider* m_pColliderCom{};

private:
	HRESULT Ready_Component();

public:
	static CCineDebugTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END