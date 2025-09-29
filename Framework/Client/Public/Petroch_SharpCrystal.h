#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CPetroch_SharpCrystal final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CPetroch_SharpCrystal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPetroch_SharpCrystal(const CPetroch_SharpCrystal& Prototype);
	virtual ~CPetroch_SharpCrystal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Bind_ShaderResources();

	virtual HRESULT Render_Shadow() override;

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	HRESULT Ready_Components();
	HRESULT Ready_Sounds();

private:
	void Play_GrowSFX();
	void Play_MaxGrowSFX();
	void Play_BurstSFX();
	void Decrease_LiveTime(_float fTimeDelta);

public:
	void Set_PlaySound(_bool _bPlaySound) { m_bPlaySound = _bPlaySound; }

public:
	void Spawn(_float4x4 SpawMat);

private:	
	CShader*				m_pShaderCom { nullptr };
	CModel*					m_pModelCom { nullptr };	
	CCollider*				m_pColliderCom { nullptr };	

private:
	_float					m_fScaleInterpole{ 0.f };	
	_float					m_fCurScale{ 0.2f };
	_float					m_fMaxScale{ 1.0f };

	_float					m_fCurLiveTime{ 0.f };
	_float					m_fAdditionalLiveTime{ 0.f };
	_float					m_fMaxLiveTime{ 3.f };	

	_float					m_fTempGrowTime{ 1.f };
	_bool					m_bPlaySound{ false };
	_bool					m_bGrowOnce{ false };
	_bool					m_bExplodeOnce{ false };
	_bool					m_bGrowMax{ false };
	_float					m_fMaxGrowTime{ 2.f };
	_float					m_fMaxExplodeTime{ 2.9f };

private:
	vector<_wstring>		m_GrowSounds{};
	vector<_wstring>		m_MaxGrowSounds{};	
	vector<_wstring>		m_BurstSounds{};


public:
	static CPetroch_SharpCrystal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
