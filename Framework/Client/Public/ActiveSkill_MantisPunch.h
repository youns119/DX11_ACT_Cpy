#pragma once

#include "ActiveSkill.h"

BEGIN(Client)

class CSkill_Topoda;

class CActiveSkill_MantisPunch
	: public CActiveSkill
{
	using super = CActiveSkill;

private:
	CActiveSkill_MantisPunch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActiveSkill_MantisPunch(const CActiveSkill_MantisPunch& Prototype);
	virtual ~CActiveSkill_MantisPunch() = default;

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

private :
	CSkill_Topoda* m_pTopoda{};

private :
	HRESULT Ready_PartObject();

public:
	static CActiveSkill_MantisPunch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END