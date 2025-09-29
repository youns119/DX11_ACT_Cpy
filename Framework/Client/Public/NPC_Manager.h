#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CNPC_Manager final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CNPC_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Manager(const CNPC_Manager& Prototype);
	virtual ~CNPC_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	_int Load_State(_wstring strNPCKey);
	void Save_State(_wstring strNPCKey, _int iState);

private:
	map<_wstring, _int> m_mapState;

public:
	static CNPC_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
