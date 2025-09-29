#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(MapTool)
// ��ȣ�ۿ� ���� �� ������Ʈ���� �� �ٲ㰡�鼭 ����� Ŭ������
class CMagnet final : public CGameObject
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
	CMagnet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMagnet(const CMagnet& _Prototype);
	virtual ~CMagnet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	_float3 Catmull_rom(_float3 vPos0, _float3 vPos1, _float3 vPos2, _float3 vPos3, _float fInterpolationRate);
	//virtual void On_CollisionEnter(CGameObject* pGameObject, _uint iGroup) override;

private:
	CShader* m_pShaderCom{};
	CModel* m_pModelCom{};
	CCollider* m_pColliderCom = { nullptr };

	_wstring m_strName = L"";

	//_bool m_bCollision = { false };
	_bool m_bPointAdd = { false };

	_float m_fInterpolationRate = {}; // ������
	_uint m_iSegmentIndex = {}; // ����Ʈ �δڽ�
	_float m_fSpeed = { 2.f }; // �򤸱�ӵ�
	_float m_fPausedTime = { 2.f }; // ���ñ⳪
	_float m_fElapsedTime = {};
	_bool m_bPaused = { true }; // ������
	_bool m_bFoward = { true }; // �ո�޸�

	vector<_float3> m_ControlPoint;
	_bool m_bMove = { false };


private:
	HRESULT Ready_Components();

public:
	static CMagnet* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCloneable* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
