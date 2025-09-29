#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CComponent;

/* 1.사본 객체들을 분류해놓은 하나의 묶음, 덩어리  */

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	void Add_GameObject(CGameObject* pGameObject) { m_GameObjects.push_back(pGameObject); }

	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Final_Update(_float fTimeDelta);

	const list<CGameObject*>& Get_GameObjects() const { return m_GameObjects; }

	class CComponent* Find_Component(const _wstring& strComponentTag, _uint iIndex);
	class CComponent* Find_Component(const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex);
	class CComponent* Find_Component_In_PartObject(const vector<_wstring>& strPartTags, const _wstring& strComponentTag, _uint iIndex);

	void Delete_GameObject(_uint iIndex);
	void Delete_GameObject(CGameObject* pGameObject);
	void Delete_GameObject(const vector<_wstring>& strPartTags, _uint iIndex);

	CGameObject* Find_GameObject(_uint iIndex);
	CGameObject* Find_GameObject(const _wstring& strName);

private:
	std::list<CGameObject*> m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END
