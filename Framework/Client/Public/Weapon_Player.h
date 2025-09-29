#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "FWeapon.h"
#include "FShell.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;

END

BEGIN(Client)

class CPlayer;
class CShell_Part;

class CWeapon_Player final
	: public CGameObject
{
	using super = CGameObject;

public :
	enum class SOCKET
	{
		SOCKET_HAND,
		SOCKET_CLAW,
		SOCKET_CLAW_END,
		SOCKET_FINGER,

		SOCKET_END
	};

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC PartObjectDesc{};

		_uint iLevel{};
		_float fDamage{};

		CPlayer* pOwner{};
	};

private:
	CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Player(const CWeapon_Player& Prototype);
	virtual ~CWeapon_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Get_Active() const { return m_bActive; }
	CGameObject* Get_Owner() { return m_pOwner; }

	const FWeaponStat* Get_WeaponStat_Const() const { return &m_tWeaponStat; }
	const FShellStat* Get_ShellStat_Const() const;

public :
	void Toggle_Active(_bool bActive) { m_bActive = bActive; }

public :
	HRESULT Attach_Shell(SHELL eShell, _float fHP);
	HRESULT Attach_Shell_NoSound(SHELL eShell, _float fHP);
	void Deattach_Shell();

public :
	void Collider_Toggle(_bool bToggle);
	HRESULT Upgrade_Weapon();

public:
	void MoveToSocket(SOCKET eSocket);

public :
	void Add_Socket(SOCKET eSocket, const _char* szFilePath);

private:
	CModel* m_pModelCom{};
	CShader* m_pShaderCom{};
	CCollider* m_pColliderCom{};

private :
	CShell_Part* m_pShell{};

private :
	CPlayer* m_pOwner{};

private:
	FWeaponStat m_tWeaponStat{};

	_bool m_bActive{ true };

private:
	const _float4x4* m_pSocketMatrices[static_cast<_uint>(SOCKET::SOCKET_END)]{};
	const _float4x4* m_pSocketMatrix{};

private:
	_float3 m_vTransforms[static_cast<_uint>(SOCKET::SOCKET_END)][3]{};

private :
	HRESULT Create_Model();

private:
	HRESULT Load_From_File(SOCKET eSocket, const _char* szFilePath, CModel* pModel);

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
