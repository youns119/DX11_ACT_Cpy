#pragma once

#include "Shell_Item.h"

#include "IGrapple.h"

BEGIN(Engine)

class CCollider;
class CPhysics;

END

BEGIN(Client)

class CUI_ShellDesc;

class CShell_World final
	: public CShell_Item
	, public IGrapple
{
	using super = CShell_Item;

private:
	CShell_World(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShell_World(const CShell_World& Prototype);
	virtual ~CShell_World() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public :
	virtual _vec3 Get_GrapplePos() override;
	virtual _uint Get_Type() override;

public :
	virtual void Shell_Launch(_vec3 vSrc, _vec3 vDst);
	virtual void UnLaunch();
	virtual void Stop();

private:
	CCollider* m_pColliderCom{};
	CPhysics* m_pPhysicsCom {};

	CUI_ShellDesc* m_pUIShellDesc{ nullptr };

private :
	_bool m_bStop{};
	_bool m_bGrappled{};
	_float m_fLerp{};
	_float3 m_vSrcPos{};
	_float3 m_vDstPos{};

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	virtual HRESULT Ready_Components() override;
	HRESULT Ready_PartObject();

public:
	static CShell_World* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END