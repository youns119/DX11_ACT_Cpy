#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

#include "EPlayerState.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CPlayer;

class CStateMachine_Player final
	: public CStateMachine
{
	using super = CStateMachine;

public:
	struct DESC
	{
		super::DESC StateMachineDesc{};

		PlayerStateFlags* pPlayerState{};
	};

private:
	CStateMachine_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine_Player(const CStateMachine_Player& Prototype);
	virtual ~CStateMachine_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;

public:
	virtual HRESULT ChangeState(const _wstring& strState) override;

private:
	CModel* m_pModel{};

private:
	CPlayer* m_pPlayer{};

private:
	PlayerStateFlags* m_pPlayerState{};

private:
	virtual _bool Check_AnimGraph(const _wstring& strAnimState) override;

private:
	virtual HRESULT Ready_AnimState() override;

public:
	static CStateMachine_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END