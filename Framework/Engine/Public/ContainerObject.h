#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CPartObject;

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC GameObjectDesc {};
	};

protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	CComponent* Find_Component_In_PartObject(const _wstring& strPartTag, const _wstring& strComponentTag);

protected:
	map<const _wstring, CPartObject*>		m_PartObjects;

protected:
	HRESULT Add_PartObject(const _wstring& strPartTag, _uint iPrototypeLevelID, const _wstring& strPrototypeTag, void* pArg = nullptr);
	CPartObject* Find_PartObject(const _wstring& strPartTag);

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END
