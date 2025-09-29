#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysics;
END

BEGIN(Client)
class CDummy_NonAnim final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc {};
		_wstring strModelTag;
		LEVELID eLevelID { LEVEL_STATIC };
	};

private:
	CDummy_NonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_NonAnim(const CDummy_NonAnim& Prototype);
	virtual ~CDummy_NonAnim() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	HRESULT Ready_Components(const _wstring& strModelTag, LEVELID eLevelID);

private:
	CShader* m_pShaderCom { nullptr };
	CModel* m_pModelCom { nullptr };
	CPhysics* m_pPhysicsCom { nullptr };

private:
	HRESULT Ready_PhysicsMesh();
	HRESULT Ready_PhysicsSphere();
	HRESULT Ready_PhysicsConvex();

public:
	static CDummy_NonAnim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
