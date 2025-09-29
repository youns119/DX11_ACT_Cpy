#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysics;
class CEffectObject;
END

BEGIN(Client)
// 상호작용 없는 맵 오브젝트들을 모델 바꿔가면서 사용할 클래스임
class CStreamVent final : public CGameObject
{
	using super = CGameObject;

public:
	//enum PHYSICS_STATE { NONE, CONVEX, TRIANGLE };
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
		//_uint				  ePhysicsType = {};
	};
	enum STATE { WAITING, STREAMING, COOLDOWN };
	enum EFFECT { BUBBLE, SMOKE, EFFECT_END };

private:
	CStreamVent(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CStreamVent(const CStreamVent& _Prototype);
	virtual ~CStreamVent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	void Call_SteamEffect();
	void LoopEnd_SteamEffect();

private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};
	CPhysics* m_pPhysicsCom{};
	CCollider* m_pColliderCom{};

	_wstring m_strName = L"";

	_uint m_iVentNum = {};
	_bool m_bFirst = true;
	_float m_fAccTimeDelta = 0.f;
	STATE m_eState = STATE::WAITING;

	_float4x4 m_ColliderMatrix{};
	_bool m_bRender = false;
private:
	CEffectObject* m_pEffect[EFFECT_END];
private:
	HRESULT Ready_Components();

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	static _uint s_iVentNum;

public:
	static CStreamVent* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
