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
	unordered_map<_uint64, bool> m_CollisionMap; // std::set 두개 만들어서 이전프레임, 현재 프레임 비교하면서 하려고 햇는데 좀 이상한가봐

	array<vector<pair<CGameObject*, CCollider*>>, LAYER_END> m_ColliderGroup {}; // 게임 오브젝트 하나에 딱 하나의 콜라이더 였다면 굳이 콜라이더 용 레이어를 만들진 않았을거야

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

