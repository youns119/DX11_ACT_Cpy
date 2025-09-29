#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
// 상호작용 없는 맵 오브젝트들을 모델 바꿔가면서 사용할 클래스임
class CSpong final : public CGameObject
{
	using super = CGameObject;

public:
	//enum PHYSICS_STATE { NONE, CONVEX, TRIANGLE };
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
		//_uint				  ePhysicsType = {};
	};

private:
	CSpong(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSpong(const CSpong& _Prototype);
	virtual ~CSpong() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	virtual void On_CollisionEnter(CGameObject* pGameObject, _uint iGroup) override;

private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};
	CCollider* m_pColliderCom = { nullptr };

	_wstring m_strName = L"";

	_float m_fAccTimeDelta = {};
	_bool m_bCollision = { false };
private:
	HRESULT Ready_Components();

public:
	static CSpong* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
