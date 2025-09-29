#pragma once

#include "ToolAnimation_Defines.h"

#include "GameObject.h"

#include "EPath.h"

BEGIN(ToolAnimation)

class CCameraDebug_Tool;

class CCineActor_Tool final
	: public CGameObject
{
	using super = CGameObject;

public:
	struct CINEACTOR
	{
		_string strName{};
		PATHTYPE ePathType{};

		_float fTime{};
		_float fLerp{};

		LERPTYPE eLerpType{};

		_float3 vBazierControl{};

		_float3 vCatmullRomControlStart{};
		_float3 vCatmullRomControlEnd{};
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		_float3 vPosition{};
		_float3 vRotation{};

		CINEACTOR tCineActor{};
	};

private:
	CCineActor_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCineActor_Tool(const CCineActor_Tool& Prototype);
	virtual ~CCineActor_Tool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	CINEACTOR* Get_CineActor() { return &m_tCineActor; }
	CTransform* Get_Transform() { return m_pTransformCom; }

public :
	void Set_PathType(_uint iPathType);

	void Set_LerpType(_uint iLerpType);

	void Set_BazierPos(_fvector vPos);
	void Set_CatmullRomStart(_fvector vPos);
	void Set_CatmullRomEnd(_fvector vPos);

public:
	void Change_ColliderColor(_bool bChange);

private:
	CCollider* m_pColliderCom{};

private :
	CCameraDebug_Tool* m_pBazier{};
	CCameraDebug_Tool* m_pCatmullRomStart{};
	CCameraDebug_Tool* m_pCatmullRomEnd{};

private :
	CINEACTOR m_tCineActor{};

private:
	_float3 m_vDefaultColor{ 0.f, 1.f, 0.f };
	_float3 m_vChangeColor{ 1.f, 0.f, 1.f };

private:
	HRESULT Ready_Component();
	HRESULT Ready_PartObject();

public:
	static CCineActor_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END