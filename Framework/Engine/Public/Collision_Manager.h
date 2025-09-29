#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider;
class CGameObject;

union COLLISION_ID
{
	struct
	{
		_uint iLeft;
		_uint iRight;
	};

	_uint64 iCollisionID;
};


class CCollision_Manager final : public CBase
{
	using super = CBase;

	enum
	{
		LAYER_END = sizeof(_uint) * 8
	};

private:
	CCollision_Manager() = default;
	virtual ~CCollision_Manager() = default;

public:
	void Update();

	void Set_CollisionEnableGroup(_uint iSrcGroup, _uint iDstGroup, _bool isEnable);
	void Add_ColliderGroup(CGameObject* pGameObject, CCollider* pCollider, _uint iGroup);

	void Clear();

	_bool Ray_Cast_To_Collider_In_ColliderGroup(_fvector vRayOrigin, _fvector vRayDirection, _float fRayLength, _Out_ CGameObject*& pHit, _Out_ _float& fDist, _uint iColliderGroup);
#
private:
	array<_uint, LAYER_END> m_CollisionGroupMatrix {};
	unordered_map<_uint64, bool> m_CollisionMap; // std::set �ΰ� ���� ����������, ���� ������ ���ϸ鼭 �Ϸ��� �޴µ� �� �̻��Ѱ���

	array<vector<pair<CGameObject*, CCollider*>>, LAYER_END> m_ColliderGroup {}; // ���� ������Ʈ �ϳ��� �� �ϳ��� �ݶ��̴� ���ٸ� ���� �ݶ��̴� �� ���̾ ������ �ʾ����ž�

private:
	void Check_Collision_Between_Group(_uint iSrcLayer, _uint iDstLayer);
	void Process_Collision(pair<CGameObject*, CCollider*> SrcPair, pair<CGameObject*, CCollider*> DstPair, _uint iSrcGroup, _uint iDstGroup);
	void Process_DeactiveCollision(pair<CGameObject*, CCollider*> SrcPair, pair<CGameObject*, CCollider*> DstPair, _uint iSrcGroup, _uint iDstGroup);
	void Process_DeadCollision(pair<CGameObject*, CCollider*> SrcPair, pair<CGameObject*, CCollider*> DstPair, _uint iSrcGroup, _uint iDstGroup);
	_bool Is_Collision(CCollider* pSrcCollider, CCollider* pDstCollider);

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

END

