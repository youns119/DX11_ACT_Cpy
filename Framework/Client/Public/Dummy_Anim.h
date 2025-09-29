#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CDummy_Anim final : public CGameObject
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
	CDummy_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_Anim(const CDummy_Anim& Prototype);
	virtual ~CDummy_Anim() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Ready_Components(const _wstring& strModelTag, LEVELID eLevelID);

private:
	CShader* m_pShaderCom { nullptr };
	CModel* m_pModelCom { nullptr };

	_uint m_iAnimIndex { 0 };

public:
	static CDummy_Anim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
