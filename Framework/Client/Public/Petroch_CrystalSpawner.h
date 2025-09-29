#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CMovement;
class CPhysics_Controller;
END

BEGIN(Client)
class CPetroch_SharpCrystal;

class CPetroch_CrystalSpawner final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
		CGameObject* pOwner{ nullptr };
	};

private:
	CPetroch_CrystalSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPetroch_CrystalSpawner(const CPetroch_CrystalSpawner& Prototype);
	virtual ~CPetroch_CrystalSpawner() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	HRESULT Ready_Components();
	HRESULT Ready_Physics();
	HRESULT Ready_Crystals();
	HRESULT Ready_Sounds();

private:	
	void Play_SpawnSFX();
	void Spawn_Crystal(_float fTimeDelta);		
	_vec2 Compute_CardinalSpline(const _vec2& p0, const _vec2& p1, _float fFactor, _float fTension);

public:
	void Set_ParentWorldMatrix(const _float4x4* ParentWorldMatrix) { m_pParentWorldMatrix = ParentWorldMatrix; }
	void Set_SpawnOption(_float4x4* vStartPos, _vector vTargetPos, _float fTension, _float fSpawnSpeed, _float fMoveSpeed, _bool bMoveLeft, _float fSpawnCount);

private:
	void Deactive();
	void Active();

private:
	CGameObject*					m_pOwner{ nullptr };
	CMovement*						m_pMovementCom{ nullptr };
	CTransform*						m_pPlayerTransform{ nullptr };
	CPhysics_Controller*			m_pPhysicsControllerCom{ nullptr };

private:	
	_int							m_iCrystalCount{ 25 };
	vector<CPetroch_SharpCrystal*>	m_Crystals{};

private:	
	_bool							m_bIsLeft{ false };
	_float							m_fDisatance{ 0.f };
	_float							m_fCurSplineFactor{ 0.f };
	_float							m_fCurSplineTension{ 0.f };

private:
	_int							m_iCurSpawnIndex{ 0 };
	_int							m_iMaxSpawnAmount{ 10 };
	_int							m_iCurSpawnAmount{ 0 };		
	_float							m_fMoveSpeed{ 1.f };	
	_float							m_fSpawnPerDistance{ 0.1f };

private:
	_float4							m_vDefaultLook{};

private:
	_float4							m_fStartPos{};
	_float4							m_fTargetPos{};	

private:
	vector<_wstring>				m_SpawnSounds{};

public:
	static CPetroch_CrystalSpawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END