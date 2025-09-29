#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysics;
END

BEGIN(Client)
// ��ȣ�ۿ� ���� �� ������Ʈ���� �� �ٲ㰡�鼭 ����� Ŭ������
class CRotatePipe final : public CGameObject
{
	using super = CGameObject;

public:
	//enum PHYSICS_STATE { NONE, CONVEX, TRIANGLE };
	struct DESC
	{
		CGameObject::DESC	  GameObjectDesc{};
		//_uint				  ePhysicsType = {};
	};

private:
	CRotatePipe(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRotatePipe(const CRotatePipe& _Prototype);
	virtual ~CRotatePipe() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};
	CPhysics* m_pPhysicsCom{};


	_float m_fAccTimeDelta = {};
	_float4x4 m_OriginMat = {};

	_wstring m_strName = L"";
	_bool m_bRender = { false };

private:
	HRESULT Ready_Components();

public:
	static CRotatePipe* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
