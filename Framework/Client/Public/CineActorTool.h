#pragma once

#include "Client_Defines.h"

#include "CineActor.h"

BEGIN(Client)

class CCineDebugTool;

class CCineActorTool final
	: public CCineActor
{
	using super = CCineActor;

private:
	CCineActorTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCineActorTool(const CCineActorTool& Prototype);
	virtual ~CCineActorTool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_PathType(_uint iPathType);

	void Set_LerpType(_uint iLerpType);

	void Set_BazierPos(_fvector vPos);
	void Set_CatmullRomStart(_fvector vPos);
	void Set_CatmullRomEnd(_fvector vPos);

public:
	void Change_ColliderColor(_bool bChange);

private:
	CCollider* m_pColliderCom{};

private:
	CCineDebugTool* m_pBazier{};
	CCineDebugTool* m_pCatmullRomStart{};
	CCineDebugTool* m_pCatmullRomEnd{};

private:
	_float3 m_vDefaultColor{ 0.f, 1.f, 0.f };
	_float3 m_vChangeColor{ 1.f, 0.f, 1.f };

private:
	HRESULT Ready_Component();
	HRESULT Ready_PartObject();

public:
	static CCineActorTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END