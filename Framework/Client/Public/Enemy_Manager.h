#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
 
BEGIN(Client)

class CEnemy_Manager: public CGameObject
{
	using super = CGameObject;

	struct ENEMY_OBJECT {

		CGameObject* EnemyObject;
		CTransform* EnemyTransform;
	};

protected:
	CEnemy_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Manager(const CEnemy_Manager& Prototype);
	virtual ~CEnemy_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Respawn_All_Enemies();

private:
	list<ENEMY_OBJECT> m_EnemyObjects{};
	CTransform* m_pPlayerTransform{nullptr};

public:
	static CEnemy_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free() override;
};
END	