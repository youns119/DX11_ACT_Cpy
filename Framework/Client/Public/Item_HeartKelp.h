#pragma once

#include "Client_Defines.h"

#include "Item.h"

BEGIN(Engine)

class CModel;
class CShader;

END

BEGIN(Client)

class CPlayer;

class CItem_HeartKelp final
	: public CItem
{
	using super = CItem;

public :
	enum class SOCKET
	{
		SOCKET_FINGER,

		SOCKET_END
	};

	struct DESC
	{
		CItem::DESC ItemDesc{};

		CPlayer* pOwner{};
	};

private:
	CItem_HeartKelp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_HeartKelp(const CItem_HeartKelp& Prototype);
	virtual ~CItem_HeartKelp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta);
	virtual HRESULT Render() override;

public:
	void MoveToSocket(SOCKET eSocket);

public:
	void Add_Socket(SOCKET eSocket, const _char* szFilePath);

private:
	HRESULT Load_From_File(SOCKET eSocket, const _char* szFilePath, CModel* pModel);

private:
	CModel* m_pModelCom{};
	CShader* m_pShaderCom{};

private:
	CPlayer* m_pOwner{};

private:
	const _float4x4* m_pSocketMatrices[static_cast<_uint>(SOCKET::SOCKET_END)]{};
	const _float4x4* m_pSocketMatrix{};

private:
	_float3 m_vTransforms[static_cast<_uint>(SOCKET::SOCKET_END)][3]{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItem_HeartKelp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free() override;
};

END