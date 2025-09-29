#pragma once

#include "ActiveSkill.h"

BEGIN(Client)

class CActiveSkill_BobbitTrap
	: public CActiveSkill
{
	using super = CActiveSkill;

private:
	CActiveSkill_BobbitTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActiveSkill_BobbitTrap(const CActiveSkill_BobbitTrap& Prototype);
	virtual ~CActiveSkill_BobbitTrap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual void Final_Update(_float fTimeDelta);
	HRESULT Render() override;

public:
	virtual void Use_Skill();

private:
	HRESULT Ready_PartObject();

public:
	static CActiveSkill_BobbitTrap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END