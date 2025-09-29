#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(MapTool)
class CShellHouse final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
		_int				  iSheleportID{ -1 };
	};

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

	void Set_SheleportID(_uint iID) { m_iSheleportID = (_int)iID; }
	_int Get_SheleportID() { return m_iSheleportID; }

private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};
	CCollider* m_pColliderCom = { nullptr };

	_int m_iSheleportID = { -1 };

private:
	HRESULT Ready_Components();

public:
	static CShellHouse* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};
END