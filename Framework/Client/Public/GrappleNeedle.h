#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "IGrapple.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CGrappleNeedle final 
	: public CGameObject
	, public IGrapple
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
	};

private:
	CGrappleNeedle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGrappleNeedle(const CGrappleNeedle& _Prototype);
	virtual ~CGrappleNeedle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	virtual _vec3 Get_GrapplePos() override;
	virtual _uint Get_Type() override;

private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};
	CCollider* m_pColliderCom = { nullptr };


private:
	HRESULT Ready_Components();

public:
	static CGrappleNeedle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};
END