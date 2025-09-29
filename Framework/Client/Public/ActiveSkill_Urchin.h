#pragma once

#include "ActiveSkill.h"

BEGIN(Client)

class CActiveSkill_Urchin
	: public CActiveSkill
{
	using super = CActiveSkill;

private:
	CActiveSkill_Urchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActiveSkill_Urchin(const CActiveSkill_Urchin& Prototype);
	virtual ~CActiveSkill_Urchin() = default;

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

public:
	static CActiveSkill_Urchin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END