#pragma once

#include "Client_Defines.h"

#include "Item.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;

END

BEGIN(Client)

class CUI_Manager;
class CCutScene_Manager;

class CItem_BottleCap final
	: public CItem
{
	using super = CItem;

public :

	struct DESC
	{
		CItem::DESC ItemDesc{};
	};

private:
	CItem_BottleCap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_BottleCap(const CItem_BottleCap& Prototype);
	virtual ~CItem_BottleCap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta);
	virtual HRESULT Render() override;

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	CUI_Manager* m_pUIManager = {nullptr};
	CCutScene_Manager* m_pCutSceneManager = {nullptr};

private:
	HRESULT Ready_Components();

public:
	static CItem_BottleCap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free() override;
};

END