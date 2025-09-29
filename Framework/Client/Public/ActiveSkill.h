#pragma once

#include "Client_Defines.h"

#include "GameObject.h"

#include "ESkill.h"
#include "FSkill.h"

BEGIN(Client)

class CPlayer;

class CActiveSkill
	: public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		GAMEOBJECT_DESC tGameObjectDesc{};

		CPlayer* pOwner{};

		ACTIVESKILL eActiveSkill{ ACTIVESKILL::ACTIVESKILL_END };
	};

protected :
	CActiveSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActiveSkill(const CActiveSkill& Prototype);
	virtual ~CActiveSkill() = default;

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

protected:
	CPlayer* m_pOwner{};

protected :
	FActiveSkill m_tActiveSkill{};

public:
	virtual CCloneable* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END