#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "FData.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysics;
class CEffectObject;
END

BEGIN(Client)

class CShellHouse final : public CGameObject
{
	using super = CGameObject;

public:
	struct DATA
	{
		FSheleportData SheleportData = {};

		_uint iLevelIndex = {};
		_float3 vPosition = {};
		_float3 vLook = {};
		_float3 vRight = {};
	};

	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
		_int				  iSheleportID{ -1 };
	};

public:
	enum SHELLEFFECT {WAIT_WIND00, WAIT_WIND01, WAIT_CIRCLE, EFFECT_END};
	enum SHELLSTATE {WAIT, ACTIVATING, ACTIVATED, STATE_END};

private:
	CShellHouse(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CShellHouse(const CShellHouse& _Prototype);
	virtual ~CShellHouse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;
	
	DATA	Get_Data() const { return m_tData; }

public :
	_bool	Check_Activated();

private:
	void	Change_State(SHELLSTATE eState);
	void	Call_Effect_Trigger(_float fTimeDelta);

private:
	CShader*				m_pShaderCom{};
	CModel*					m_pModelCom{};
	CCollider*				m_pColliderCom = { nullptr };
	CPhysics*				m_pPhysicsCom{ nullptr };

	static map<_int, _bool> s_ActivatedShells;
	DATA					m_tData;

	_int m_iSheleportID = { -1 };

private:
	CEffectObject*			m_pEffects[EFFECT_END];
	SHELLSTATE				m_eState{ STATE_END };

private:
	HRESULT Ready_Components();

public:
	static CShellHouse* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};
END