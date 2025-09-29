#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysics;
END

BEGIN(Client)

class CFakeMoonBrick final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
	};

private:
	CFakeMoonBrick(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFakeMoonBrick(const CFakeMoonBrick& _Prototype);
	virtual ~CFakeMoonBrick() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	//CCollider* m_pColliderCom = { nullptr };
	CPhysics* m_pPhysicsCom{};

private:
	HRESULT Ready_Components();

public:
	static CFakeMoonBrick* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;

};

END