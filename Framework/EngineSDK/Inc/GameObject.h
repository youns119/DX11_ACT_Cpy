#pragma once

#include "Cloneable.h"
#include "Transform.h"

BEGIN(Engine)

class CGameInstance;
class CComponent;
class CTransform;
class CCollider;
class CEvent;

class ENGINE_DLL CGameObject abstract : public CCloneable
{
	using super = CCloneable;

public:
	struct GAMEOBJECT_DESC 
	{
		union
		{
			CTransform::TRANSFORM_DESC TransformDesc {};
			struct
			{
				_float		fSpeedPerSec;
				_float		fRotationPerSec;
				_float4x4	WorldMatrix;
			};
		};

		_wstring strName {L"object"};

		/* Part Object */
		const _float4x4* pParentWorldMatrix { nullptr };
	};
	using DESC = GAMEOBJECT_DESC;

protected:
//public: // 어짜피 추상 클래스라서 public으로 한다한들 문제가 될것은 없지만 찝찝해서 탈락임
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg); // Create TransformCom (if nullptr : default tranform value, else fill with pArg)

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual void Final_Update(_float fTimeDelta);

	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT Render_Revealage() { return S_OK; }
	virtual HRESULT Render_Distortion() { return S_OK; }

	CComponent* Find_Component(const _wstring& strComponentTag);

	const _wstring& Get_Name() const { return m_strName; }
	_uint Get_ID() const { return m_iID; }
	const map<const _wstring, CGameObject*>& Get_PartObjects() const { return m_PartObjects; }
	const _float4x4& Get_CombinedMatrix() { return m_CombinedWorldMatrix; }

	void Delete_PartObject(const vector<_wstring>& strPartTags, _uint iDepth = 0);

	virtual void Save_GameObject(HANDLE hFile);
	
	// 각자 알아서 상황에 맞게 다시 작성할것, virtual 이니까
	virtual vector<pair<_float, CGameObject*>> Ray_Cast_To_Collider(_fvector RayOrigin, _fvector RayDirection, _float fRayLength);
	vector<pair<CGameObject*, CCollider*>> Intersect_Collider(CCollider* pOtherCollider);
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) {}
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) {}
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) {}

	_float Calculate_Distance_From(_vector vPosition);

	_bool Get_IsDead() const { return m_isDead; }
	void Set_IsDead() { m_isDead = true; }
	_bool Get_IsActive() const { return m_isActive; }
	void Set_IsActive(_bool isActive) { m_isActive = isActive; }

	_bool Get_IsLag() const { return m_bTimeLag; }
	void Set_IsLag(_bool bTimeLag) { m_bTimeLag = bTimeLag; }

/* Container Object */
public:
	CComponent* Find_Component_In_PartObject(const vector<_wstring>& strPartTags,const _wstring& strComponentTag);

/* Container Object */
public:
	HRESULT Add_PartObject(const _wstring& strPartTag, _uint iPrototypeLevelID, const _wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Find_PartObject(const vector<_wstring>& strPartTags, _uint iDepth = 0);

/* Blend Object */
public:
	_float Get_CamDistance() const { return m_fCamDistance; }

	// Event
public :
	HRESULT Call_Event(const _char* szEventTag);

protected:
	ID3D11Device*			m_pDevice{ nullptr };
	ID3D11DeviceContext*	m_pContext{ nullptr };
	CGameInstance*			m_pGameInstance{ nullptr };

	std::map<const _wstring, CComponent*> m_Components;
	CTransform*		m_pTransformCom{ nullptr };

	_wstring		m_strName { L"No_Name" };
	_uint			m_iID { 0 };
	static _uint	s_iID;

	_bool		m_isDead { false };
	_bool		m_isActive { true };

	_bool m_bTimeLag{ true };

/* Container Object */
protected:
	map<const _wstring, CGameObject*>		m_PartObjects;
/* Part Object */
protected:
	const _float4x4* m_pParentWorldMatrix { nullptr };
	_float4x4 m_CombinedWorldMatrix {};

/* Blend Object */
protected:
	_float m_fCamDistance { 0.f };

	// Event
protected :
	map<const _wstring, CEvent*> m_mapEvent;

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

	HRESULT Delete_Component(const _wstring& strComponentTag, CComponent** ppOut);

	HRESULT Bind_ObjectID_To_Shader(class CShader* pShader, const _char* pConstantName) const;

/* Blend Object */
protected:
	void Compute_CamDistance();

	// Event
protected :
	HRESULT Add_Event(const _wstring& strEventTag, CEvent* pEvent);
	CEvent* Find_Event(const _wstring& _strEventTag);

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
