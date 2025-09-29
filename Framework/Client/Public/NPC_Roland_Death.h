#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CCineCamera;
END

BEGIN(Client)

class CUI_Manager;
class CCutScene_Manager;

class CNPC_Roland_Death final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_HIT,
		ANIM_DIE,
	};

	enum STATE
	{
		STATE_HIT,
		STATE_DIE,
		STATE_DOWN,
	};

	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CNPC_Roland_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Roland_Death(const CNPC_Roland_Death& Prototype);
	virtual ~CNPC_Roland_Death() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	void Shake();
	void Move();

private:
	void Call_SparkEffect();
	void Call_FallEffect();

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };

	CUI_Manager* m_pUIManager = { nullptr };
	CCineCamera* m_pCineCamera = { nullptr };
	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	const _float3		m_vStartPosition = { 25.47f, 33.82f, 21.81f };

	_bool		m_isRender = { false };
	_bool		m_isCutSceneStart = { false };
	_bool		m_isCutSceneEnd = { false };

	STATE		m_eCurState = { STATE_HIT };

	vector<_float3> m_vPos;
	_int			m_iIndex = { 0 };

	_bool		m_bAnimStop = { false };

	_float m_fFrame = {0.f};

	_bool		m_bFallEffectCalled = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CNPC_Roland_Death* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END