#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CCutScene_Manager;

class CNPC_DanceCrab final : public CGameObject
{
	using super = CGameObject;

public:
	enum ANIM
	{
		ANIM_CLAP,
		ANIM_DANCE,
		ANIM_JUMP,
		ANIM_ROAR,
	};
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CNPC_DanceCrab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_DanceCrab(const CNPC_DanceCrab& Prototype);
	virtual ~CNPC_DanceCrab() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	HRESULT Ready_Components();

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

	CCutScene_Manager* m_pCutSceneManager = { nullptr };

	_bool m_bAnimStop = { false };
	_uint m_iDanceIndex = {};
public:
	static CNPC_DanceCrab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END