#pragma once

#include "Client_Defines.h"

#include "GameObject.h"

#include "EProjectile.h"
#include "FProjectile.h"

BEGIN(Client)

class CProjectile;

class CProjectile_Manager final
	: public CGameObject
{
	using super = CGameObject;

public :
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private :
	CProjectile_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Manager(const CProjectile_Manager& Prototype);
	virtual ~CProjectile_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	CProjectile* Get_Projectile(PROJECTILE eProjectile);

public :
	void Spawn(PROJECTILE eProjectile, _uint iNum, const FProjectileStat& tProjectileStat);

public :
	void Deactivate_All();

private :
	vector<CProjectile*> m_vecProjectile[static_cast<_uint>(PROJECTILE::PROJECTILE_END)];

private :
	HRESULT Ready_Projectiles();

public :
	static CProjectile_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free() override;
};

END